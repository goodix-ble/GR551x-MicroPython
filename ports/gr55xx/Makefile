include ../../py/mkenv.mk

# must be cross compile
CROSS = 1
V = 1
BUILD_VERBOSE = 1
MICROPY_FATFS = 1

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

# set compiler
CROSS_COMPILE = arm-none-eabi-

# set target name
TARGET_NAME := gr5515
TARGET_APP  := gr5515_app

# set include dirs
INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

INC_GR += -I.
INC_GR += -I./boards
INC_GR += -I./boards/gr5515
INC_GR += -I./boards/gr5515/system
INC_GR += -I./boards/drivers
INC_GR += -I./GR551x_SDK_V1_00/build/config
INC_GR += -I./GR551x_SDK_V1_00/components/app_drivers/inc
INC_GR += -I./GR551x_SDK_V1_00/components/boards
INC_GR += -I./GR551x_SDK_V1_00/components/drivers_ext/gr551x
INC_GR += -I./GR551x_SDK_V1_00/components/sdk
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/app_alarm
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/app_assert
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/app_error
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/app_key
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/app_log
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/app_timer
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/at_cmd
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/bsp
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/gui
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/gui/gui_config
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/hal_flash
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/hal_flash/vflash
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/hci_uart
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/pmu_calibration
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/log
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/pwr_mgmt
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/ring_buffer
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/sensorsim
INC_GR += -I./GR551x_SDK_V1_00/components/libraries/utility
INC_GR += -I./GR551x_SDK_V1_00/components/patch/ind
INC_GR += -I./GR551x_SDK_V1_00/drivers/inc
INC_GR += -I./GR551x_SDK_V1_00/toolchain/cmsis/include
INC_GR += -I./GR551x_SDK_V1_00/toolchain/gr551x/include
INC_GR += -I./GR551x_SDK_V1_00/toolchain/gr551x/source/arm

# mp porting
INC_GR += -I./modules/board
INC_GR += -I./modules/utime
INC_GR += -I./modules/uos
INC_GR += -I./modules/machine


# lib
INC_GR += -I../../lib/timeutils
INC_GR += -I../../extmod

LIBS =

# mpy porting
SRC_C = modules/board/modboard.c \
        modules/board/led.c \
        modules/board/xflash.c \
        modules/utime/modutime.c \
        modules/uos/moduos.c \
        modules/machine/modmachine.c \
        modules/machine/timer.c \
        lib/timeutils/timeutils.c \
        boards/drivers/spi_flash.c \
        boards/gr5515/system/gr55xx_hal_msp.c \
        # boards/gr5515/system/gr55xx_it.c \

ifeq ($(MICROPY_FATFS), 1)
    SRC_C += \
        lib/oofatfs/ff.c \
        lib/oofatfs/ffunicode.c \
        ports/gr55xx/boards/ports/fatfs_port.c \
        
    INC_GR += -I./lib/oofatfs
endif

# ble porting
SRC_C += \
    boards/ports/ble/gr_porting.c \
    boards/ports/ble/xblepy_hal_common.c \
    boards/ports/ble/xblepy_hal_gap.c \
    boards/ports/ble/xblepy_hal_gatt_server.c \
    boards/ports/ble/user_callback/user_gatts_profile_callback.c \
    boards/ports/ble/user_callback/user_gap_callback.c \
    boards/ports/ble/user_callback/user_gatt_common_callback.c \
    boards/ports/ble/user_callback/user_gattc_callback.c \
    boards/ports/ble/user_callback/user_l2cap_callback.c \
    boards/ports/ble/user_callback/user_sm_callback.c \
    modules/ble/modble.c \
    modules/xblepy/modxblepy.c \
    modules/xblepy/xblepy_uuid.c \
    modules/xblepy/xblepy_constants.c \
    modules/xblepy/xblepy_descriptor.c \
    modules/xblepy/xblepy_characteristic.c \
    modules/xblepy/xblepy_service.c \
    modules/xblepy/xblepy_device.c \
    modules/xblepy/xblepy_peripheral.c \
    modules/xblepy/xblepy_central.c \
    modules/xblepy/xblepy_scan_entry.c \
    modules/xblepy/xblepy_scanner.c \
    modules/xblepy/xblepy_delegate.c \
    modules/xblepy/xblepy_default_gap_delegate.c \
    modules/xblepy/xblepy_default_gatts_delegate.c \

    
INC_GR += -I./modules/ble
INC_GR += -I./modules/xblepy
INC_GR += -I./boards/ports/ble
INC_GR += -I./boards/ports/ble/user_callback


SRC_C += \
	main.c \
    help.c \
	uart_core.c \
    mphalport.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \
    GR551x_SDK_V1_00/toolchain/gr551x/source/interrupt_gr55xx.c \
    GR551x_SDK_V1_00/toolchain/gr551x/source/platform_gr55xx.c \
    GR551x_SDK_V1_00/toolchain/gr551x/source/system_gr55xx.c \
    GR551x_SDK_V1_00/components/libraries/app_assert/app_assert.c  \
    GR551x_SDK_V1_00/components/libraries/app_error/app_error.c  \
    GR551x_SDK_V1_00/components/libraries/app_log/app_log.c  \
    GR551x_SDK_V1_00/components/libraries/app_timer/app_timer.c  \
    GR551x_SDK_V1_00/components/libraries/pmu_calibration/pmu_calibration.c  \
    GR551x_SDK_V1_00/components/libraries/ring_buffer/ring_buffer.c  \
    GR551x_SDK_V1_00/components/libraries/utility/utility.c  \
    GR551x_SDK_V1_00/components/app_drivers/src/app_adc.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_comp.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_dual_tim.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_hmac.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_i2s.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_pwr_mgmt.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_rng.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_systick.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_uart.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_aes.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_dma.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_gpiote.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_i2c.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_io.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_qspi.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_spi.c \
    GR551x_SDK_V1_00/components/app_drivers/src/app_tim.c \

    # GR551x_SDK_V1_00/components/libraries/hal_flash/hal_flash.c  \
    # GR551x_SDK_V1_00/components/libraries/hal_flash/vflash/vflash.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_comp.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_hmac.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_pwr.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_wdt.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_dma.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_pkc.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_adc.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_cortex.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_i2c.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_qspi.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_xqspi.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_dual_tim.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_pwm.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_aes.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_dma.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_i2s.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_rng.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_adc.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_gpio.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_rng.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_aon_gpio.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_dual_tim.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_msio.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_spi.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_aes.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_hmac.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_spi.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_aon_wdt.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_efuse.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_tim.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_aon_gpio.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_i2c.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_tim.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_calendar.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_exflash.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_pkc.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_uart.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_cgc.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_i2s.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_uart.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_cgc.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_gpio.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_pwm.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_hal_utils.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_comp.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_msio.c  \
    # GR551x_SDK_V1_00/drivers/src/gr55xx_ll_xqspi.c  \

    
SRC_ASM = GR551x_SDK_V1_00/toolchain/gr551x/source/gcc/startup_gr55xx.s

# set CFLAGS
# -DNDEBUG - close debug & assert
CFLAGS += $(INC) $(INC_GR)
CFLAGS += -DGR5515_SK
CFLAGS += -DAPP_DRIVER_USE_ENABLE=1
# CFLAGS += -O0 -std=gnu99 
CFLAGS += -ggdb3
CFLAGS += -ffunction-sections -fdata-sections 
CFLAGS += -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mthumb-interwork -mthumb -mcpu=cortex-m4
CFLAGS += -DFFCONF_H=\"ports/gr55xx/boards/ports/ffconf.h\"

# set LDFLAGS
LDFLAGS += -Wl,--gc-sections 
# -Wa,-mimplicit-it=thumb
# -Map=$(BUILD)/$(TARGET_NAME).map
# $(@:.axf=.map) --xref
LDFLAGS += -L./GR551x_SDK_V1_00/components/sdk/linker/lib_gcc -lsdk

# set ref files
LINK_SCRIPT := ./GR551x_SDK_V1_00/toolchain/gr551x/source/gcc/gcc_linker.lds
PATCH_FILE  += ./GR551x_SDK_V1_00/components/sdk/linker/symbol/sdk_rom_symbol_gcc.txt
TOOL_PATH    = ./GR551x_SDK_V1_00/build/binaries/

# set OBJ
## GR_SYS_OBJ += ./boards/gr5515/lib/gr55xx_hal_exflash.o
## GR_SYS_OBJ += ./boards/gr5515/lib/gr55xx_hal_pwr.o
## GR_SYS_OBJ += ./boards/gr5515/lib/gr55xx_hal_xqspi.o
## GR_SYS_OBJ += ./boards/gr5515/lib/gr55xx_ll_xqspi.o
# GR_SYS_OBJ += ./boards/gr5515/lib/svc_handler_gcc.o

#C_OBJ   = $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) 
#ASM_OBJ = $(addprefix $(BUILD)/, $(SRC_ASM:.s=.o))
OBJ     = $(PY_CORE_O) $(PY_EXTMOD_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) $(addprefix $(BUILD)/, $(SRC_ASM:.s=.o))

# OBJ_DIR = $(BUILD)
# OBJ_2 = $(patsubst %.o,$(OBJ_DIR)/%.o,$(notdir $(OBJ)))

# verbosity switch
V ?= 0
ifeq ($(V),0)
	V_CC = @echo "  arm-none-eabi-CC    " $@;
	V_CPP = @echo "  arm-none-eabi-CPP   " $@;
	V_LINK = @echo "  arm-none-eabi-LINK  " $@;
	V_OBJCOPY = @echo "  arm-none-eabi-OBJCPY" $@;
	V_CLEAN = @echo "  arm-none-eabi-CLEAN ";
	V_SED = @echo "  SED   " $@;
	V_GAWK = @echo "  GAWK  " $@;
else
	V_OPT = '-v'
endif

# default make goal
all: $(BUILD)/$(TARGET_APP).bin

$(BUILD)/$(TARGET_APP).bin: $(BUILD)/$(TARGET_NAME).bin
	$(ECHO) "update binary file ..."
	$(TOOL_PATH)/ble_tools.exe --cfg=./GR551x_SDK_V1_00/build/config/custom_config.h --mode=gen --bin=$(BUILD)/$(TARGET_NAME).bin --outdir=$(BUILD) --app_name=$(TARGET_APP)
	rm -rf $(BUILD)/info.* $(BUILD)/header.* $(BUILD)/*.tmp $(BUILD)/linker.lds $(BUILD)/$(TARGET_NAME).bin
    
$(BUILD)/$(TARGET_NAME).bin: $(BUILD)/$(TARGET_NAME).axf
	$(ECHO) "compile binary file ..."
	$(V_OBJCOPY)$(OBJCOPY) -O binary $< $@

$(BUILD)/$(TARGET_NAME).axf: $(OBJ)
	$(ECHO) "compile .axf file ..."
	$(V_LINK)$(CC) $(CFLAGS) -T $(LINK_SCRIPT) $(PATCH_FILE)  $(OBJ) ${GR_SYS_OBJ} $(LDFLAGS)  -Wl,-Map=$(BUILD)/$(TARGET_NAME).map -o $@

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=longlong $< > $@

deploy: $(BUILD)/$(TARGET_APP).bin
	$(ECHO) "Writing $< to the gr5515-sk board"


include $(TOP)/py/mkrules.mk
