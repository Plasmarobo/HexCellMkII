#Defines variables for a specific board/processor
# C defines
C_DEFS = \
-DUSE_HAL_DRIVER \
-DSTM32F103xB \
-DHW_VERSION=$(HW_VERSION) \
-DFW_VERSION_MAJOR=$(FW_VERSION_MAJOR) \
-DFW_VERSION_MINOR=$(FW_VERSION_MINOR) \
-DFW_VERSION_PATCH=$(FW_VERSION_PATCH) \
-DBOOT_ADDRESS=$(BOOT_ADDRESS)

FREERTOS_PORTABLE=GCC/ARM_CM3

# $(call feature_source, FEATURE, sourcefile)
define add_feature_source
$(eval
	ifneq ($(filter $(1),$(FEATURES)),)
		SOURCE_LIST+=$(2)
		$(info Feature $(1)...Enabled)
	else
		$(info Feature $(1)...Disabled)
	endif
)
endef

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc -c
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp -c
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
LD = $(GCC_PATH)/$(PREFIX)gcc
else
CC = $(PREFIX)gcc -c
AS = $(PREFIX)gcc -x assembler-with-cpp -c
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
LD = $(PREFIX)gcc
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# compile gcc flags
ASFLAGS+= $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2 -ggdb
endif

#######################################
# LDFLAGS
#######################################
# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs $(LIBDIR) $(LIBS) -L$(SOURCE_ROOT)/$(BSP_PREFIX) -Wl,--gc-sections -Xlinker --print-memory-usage

# macros for gcc
# AS defines
AS_DEFS = 

# AS includes
INCLUDE_DIRS = \
$(BSP_PREFIX) \
$(BSP_PREFIX)/Core/Inc

# C includes
INCLUDE_DIRS += \
$(BSP_PREFIX)/Core/Inc \
$(BSP_PREFIX)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
$(BSP_PREFIX)/Drivers/CMSIS/Include \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Inc \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
$(BSP_PREFIX)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
$(BSP_PREFIX)/Drivers/CMSIS/Include \
$(BSP_PREFIX)/Core/Src/$(TARGET)

######################################
# source
######################################

# C sources
SOURCE_DIRS = \
$(BSP_PREFIX)/Core/Src/$(TARGET) \
$(BSP_PREFIX)

SOURCE_LIST = \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
$(BSP_PREFIX)/Core/Src/stm32f1xx_hal_msp.c \
$(BSP_PREFIX)/Core/Src/system_stm32f1xx.c \
$(BSP_PREFIX)/Core/Src/boot.c \
$(BSP_PREFIX)/Core/Src/bsp.c \
$(BSP_PREFIX)/Core/Src/comm_phy.c \
$(BSP_PREFIX)/Core/Src/flash.c \
$(BSP_PREFIX)/Core/Src/gpio.c \
$(BSP_PREFIX)/Core/Src/keystore.c \
$(BSP_PREFIX)/Core/Src/reset.c

$(call add_feature_source,ADC,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
$(BSP_PREFIX)/Core/Src/adc.c)

$(call add_feature_source,CAN,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c \
$(BSP_PREFIX)/Core/Src/can.c)

$(call add_feature_source,CORTEX,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c)

$(call add_feature_source,CRC,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c \
$(BSP_PREFIX)/Core/Src/crc.c)

$(call add_feature_source,DMA,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
$(BSP_PREFIX)/Core/Src/dma.c)

$(call add_feature_source,RTC,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c \
$(BSP_PREFIX)/Core/Src/rtc.c)

$(call add_feature_source,SPI,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c \
$(BSP_PREFIX)/Core/Src/spi.c \
$(BSP_PREFIX)/Core/Src/fast_hsv2rgb_32bit.c \
$(BSP_PREFIX)/Core/Src/leds.c)

$(call add_feature_source,TIM,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
$(BSP_PREFIX)/Core/Src/stm32f1xx_hal_timebase_tim.c \
$(BSP_PREFIX)/Core/Src/tim.c)

$(call add_feature_source,UART,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
$(BSP_PREFIX)/Core/Src/usart.c)

$(call add_feature_source,USB,$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c)

SOURCE_WHITELIST=
SOURCE_BLACKLIST=