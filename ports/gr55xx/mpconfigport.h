#include <stdint.h>
#include <alloca.h>


#define MICROPY_HEAP_SIZE                   (48*1024)
#define MICROPY_ENABLE_COMPILER             (1)
#define MICROPY_PY_UTIME_MP_HAL             (1)
#define MICROPY_USE_INTERNAL_PRINTF         (0)

#define MICROPY_MALLOC_USES_ALLOCATED_SIZE  (1)
#define MICROPY_MEM_STATS                   (1)
#define MICROPY_STACK_CHECK                 (1)
#define MICROPY_ENABLE_PYSTACK              (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     (1)
#define MICROPY_PY_MICROPYTHON_STACK_USE    (1)


/********************************************************************
 *       builtin functions
 ********************************************************************/
#define MICROPY_PY_SYS_PLATFORM             "gr5515"
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       gr55xx_help_text
#define MICROPY_PY_BUILTINS_BYTEARRAY       (1)
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS   (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW      (1)
#define MICROPY_PY_BUILTINS_ENUMERATE       (1)
#define MICROPY_PY_BUILTINS_FILTER          (1)
#define MICROPY_PY_BUILTINS_FROZENSET       (1)
#define MICROPY_PY_BUILTINS_REVERSED        (1)
#define MICROPY_PY_BUILTINS_SET             (1)
#define MICROPY_PY_BUILTINS_SLICE           (1)
#define MICROPY_PY_BUILTINS_PROPERTY        (1)
#define MICROPY_PY_BUILTINS_MIN_MAX         (1)
#define MICROPY_PY_BUILTINS_STR_COUNT       (1)
#define MICROPY_PY_BUILTINS_STR_OP_MODULO   (1)


/********************************************************************
 *       py & core modules
 ********************************************************************/
#define MICROPY_PY_IO                       (1)
#define MICROPY_PY_IO_IOBASE                (1)
#define MICROPY_PY_IO_FILEIO                (1)
#define MICROPY_PY_IO_BYTESIO               (1)
#define MICROPY_PY_IO_BUFFEREDWRITER        (1)

#define MICROPY_PY___FILE__                 (1)
#define MICROPY_PY_GC                       (1)
#define MICROPY_PY_ARRAY                    (1)
#define MICROPY_PY_ATTRTUPLE                (1)
#define MICROPY_PY_COLLECTIONS              (1)
#define MICROPY_PY_MATH                     (1)
#define MICROPY_PY_CMATH                    (1)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS   (1)
#define MICROPY_PY_MATH_ISCLOSE             (1)
#define MICROPY_PY_MATH_FACTORIAL           (1)
#define MICROPY_PY_STRUCT                   (1)
#define MICROPY_PY_SYS                      (1)

#define MICROPY_MODULE_FROZEN_MPY           (1)
#define MICROPY_CPYTHON_COMPAT              (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_LONGLONG)
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_FLOAT)

#if MICROPY_FLOAT_IMPL > MICROPY_FLOAT_IMPL_NONE
    #define MICROPY_OBJ_REPR                (MICROPY_OBJ_REPR_C)
#endif 

/********************************************************************
 *       Support VFS 
 ********************************************************************/
#ifndef MICROPY_VFS
    #define MICROPY_VFS                     (1)
#endif
#define MICROPY_VFS_FAT                     (MICROPY_VFS)

#if MICROPY_VFS
    #define MICROPY_READER_VFS              (1)
    #define MICROPY_FATFS_RPATH             (2)
    #define MICROPY_FATFS_MAX_SS            (4096)
    #define MICROPY_FATFS_ENABLE_LFN        (1)

    // use vfs's functions for import stat and builtin open
    #define mp_import_stat                  mp_vfs_import_stat
    #define mp_builtin_open                 mp_vfs_open
    #define mp_builtin_open_obj             mp_vfs_open_obj    
#endif

/********************************************************************
 *       BLE Feature
 ********************************************************************/
#define MICROPY_PY_BLE                      (1)
#define MICROPY_PY_BLE_NUS                  (0)
#define BLUETOOTH_WEBBLUETOOTH_REPL         (0)
#define MICROPY_PY_XBLEPY                   (1)
#define MICROPY_PY_XBLEPY_CENTRAL           (0)
#define MICROPY_PY_XBLEPY_PERIPHERAL        (1)
#define MICROPY_PY_XBLEPY_DESCRIPTOR        (1)



#define MICROPY_QSTR_BYTES_IN_HASH          (1)
#define MICROPY_QSTR_EXTRA_POOL             mp_qstr_frozen_const_pool
#define MICROPY_ALLOC_PATH_MAX              (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT      (16)
#define MICROPY_EMIT_X64                    (0)
#define MICROPY_EMIT_THUMB                  (0)
#define MICROPY_EMIT_INLINE_THUMB           (0)
#define MICROPY_COMP_MODULE_CONST           (0)
#define MICROPY_COMP_CONST                  (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN    (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN    (0)
#define MICROPY_DEBUG_PRINTERS              (0)
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_GC_ALLOC_THRESHOLD          (0)
#define MICROPY_REPL_EVENT_DRIVEN           (0)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_HELPER_LEXER_UNIX           (0)
#define MICROPY_ENABLE_SOURCE_LINE          (0)
#define MICROPY_ENABLE_DOC_STRING           (0)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_PY_ASYNC_AWAIT              (0)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG       (0)



// TODO these should be generic, not bound to fatfs
#define mp_type_fileio                      mp_type_vfs_fat_fileio
#define mp_type_textio                      mp_type_vfs_fat_textio

// type definitions for the specific machine
#define MICROPY_MAKE_POINTER_CALLABLE(p)    ((void*)((mp_uint_t)(p) | 1))

#define UINT_FMT                            "%u"
#define INT_FMT                             "%d"
#define HEX2_FMT                            "%02x"
typedef int                                 mp_int_t; // must be pointer size
typedef unsigned int                        mp_uint_t; // must be pointer size
typedef long                                mp_off_t;


#define MP_PLAT_PRINT_STRN(str, len)        mp_hal_stdout_tx_strn_cooked(str, len)
#define MP_STATE_PORT                       MP_STATE_VM
#define MICROPY_PORT_ROOT_POINTERS          const char *readline_hist[8];
    

/********************************************************************
 *              builtin modules & functions declaration
 ********************************************************************/
extern const struct _mp_obj_module_t        mp_module_board;
extern const struct _mp_obj_module_t        mp_module_utime;
extern const struct _mp_obj_module_t        mp_module_uos;

#if MICROPY_PY_BLE > 0u
    extern const struct _mp_obj_module_t        mp_module_ble;    
    extern const struct _mp_obj_module_t        mp_module_xblepy;
    
    #define BLE_MODULE                          { MP_ROM_QSTR(MP_QSTR_ble), MP_ROM_PTR(&mp_module_ble) },
    #define XBLEPY_MODULE                       { MP_ROM_QSTR(MP_QSTR_xblepy), MP_ROM_PTR(&mp_module_xblepy) },
#else
    #define BLE_MODULE
    #define XBLEPY_MODULE
#endif

/*
 * builtin modules
 */
#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_uos),             MP_ROM_PTR(&mp_module_uos) }, \
    { MP_ROM_QSTR(MP_QSTR_board),           MP_ROM_PTR(&mp_module_board) }, \
    { MP_ROM_QSTR(MP_QSTR_utime),           MP_ROM_PTR(&mp_module_utime) }, \
    BLE_MODULE \
    XBLEPY_MODULE \


/*
 * builtin functions
 */
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open),            MP_ROM_PTR(&mp_builtin_open_obj) },

