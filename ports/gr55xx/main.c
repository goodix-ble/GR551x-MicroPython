#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/utils/pyexec.h"
#include "lib/mp-readline/readline.h"
#include "extmod/vfs_fat.h"

#include "mp_defs.h"
#include "mphalport.h"

//porting module
#include "led.h"
#include "xflash.h"

static char *stack_top;
#if MICROPY_ENABLE_GC
static char heap[MICROPY_HEAP_SIZE];
#endif



#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

#if MICROPY_PY_MACHINE_XFLASH > 0u
static fs_user_mount_t fs_user_mount_flash;
static uint8_t vfs_buf[FF_MAX_SS];
MP_NOINLINE STATIC bool xflash_fs_mount(void) {
    // init the vfs object
    fs_user_mount_t *vfs_fat = &fs_user_mount_flash;
    vfs_fat->flags = 0;
    xflash_init_vfs(vfs_fat);

    // try to mount the flash
    FRESULT res = f_mount(&vfs_fat->fatfs);

    if (res == FR_NO_FILESYSTEM) {
        printf("MPY: no fs, make the fatfs\n");
        res = f_mkfs(&vfs_fat->fatfs, FM_FAT, 0, vfs_buf, sizeof(vfs_buf));
        if (res != FR_OK) {
            printf("MPY: can't create flash filesystem\n");
            return FALSE;
        }

        // set label
        //f_setlabel(&vfs_fat->fatfs, "xflash");

        // populate the filesystem with factory files
        // factory_reset_make_files(&vfs_fat->fatfs);
    } else {
        printf("MPY: fs exists, load the fatfs\n");
    }

    // mount the flash device (there should be no other devices mounted at this point)
    // we allocate this structure on the heap because vfs->next is a root pointer
    mp_vfs_mount_t *vfs = m_new_obj_maybe(mp_vfs_mount_t);
    if (vfs == NULL) {
        printf("MPY: can't mount flash filesystem\n");
        return FALSE;
    }
    vfs->str = "/flash";
    vfs->len = 6;
    vfs->obj = MP_OBJ_FROM_PTR(vfs_fat);
    vfs->next = NULL;
    MP_STATE_VM(vfs_mount_table) = vfs;

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    MP_STATE_PORT(vfs_cur) = vfs;

    return TRUE;
}
#endif


int main(int argc, char **argv) {
    int stack_dummy;
    stack_top = (char*)&stack_dummy;
soft_reset:
    mp_stack_set_top(stack_top);
    
    // Make MicroPython's stack limit somewhat smaller than full stack available
#if MICROPY_STACK_CHECK > 0u
    mp_stack_set_limit(MP_GR5515_STACK_SIZE - 1024);
#endif
    mp_gr5515_init();
    
    mp_hal_stdout_tx_str("MicroPython Start...\r\n");

#if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
#endif

#if MICROPY_ENABLE_PYSTACK
    static mp_obj_t pystack[512];
    mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
#endif

    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    mp_obj_list_init(mp_sys_argv, 0);
        
    pyb_set_repl_info(MP_OBJ_NEW_SMALL_INT(0));

    readline_init0();

#if MICROPY_PY_MACHINE_LED > 0u
    mp_led_init();
    
    do_str("import board\r\n"   \
           "import utime\r\n"   \
           "board.LED(1).on()\r\n"  \
           "utime.sleep_ms(200)\r\n" \
           "board.LED(1).off()\r\n" \
           "utime.sleep_ms(200)\r\n" \
           "board.LED(1).on()\r\n"  \
           "utime.sleep_ms(200)\r\n" \
           "board.LED(1).off()\r\n" ,           
           MP_PARSE_FILE_INPUT);
#endif

#if MICROPY_PY_MACHINE_XFLASH > 0u    
    if(TRUE == xflash_fs_mount() ){
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash));
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash_slash_lib));
    }
#endif

#if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("main.py");
#endif
    pyexec_file_if_exists("main.py");

    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

    mp_deinit();    
    gc_sweep_all();
    
    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");    
    mp_hal_delay_ms(10);
    
    goto soft_reset;

    return 0;
}


void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    gc_dump_info();
}

#if !MICROPY_VFS
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    printf(">>> mp_lexer_new_from_file: to implement \r\n");
    mp_raise_OSError(MP_ENOENT);
}

// use vfs's functions for import stat and builtin open

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
#endif

void nlr_jump_fail(void *val) {
    printf(">>> FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    __fatal_error("");
}

void NORETURN __fatal_error(const char *msg) {
    printf(">>> __fatal_error, program stopped! \r\n");
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

