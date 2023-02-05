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
$(BSP_PREFIX)/USB_DEVICE/App \
$(BSP_PREFIX)/USB_DEVICE/Target \
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
$(BSP_PREFIX)/Core/Src \
$(BSP_PREFIX)/Drivers/STM32F1xx_HAL_Driver/Src \
$(BSP_PREFIX)/USB_DEVICE/App \
$(BSP_PREFIX)/USB_DEVICE/Target \
$(BSP_PREFIX)/Core/Src/$(TARGET) \
$(BSP_PREFIX)

SOURCE_WHITELIST=
SOURCE_BLACKLIST=