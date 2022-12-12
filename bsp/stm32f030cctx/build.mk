#Defines variables for a specific board/processor
# C defines
C_DEFS += \
-DUSE_HAL_DRIVER \
-DSTM32F030xC \
-DHW_VERSION=$(HW_VERSION)

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m0

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2 -ggdb
ASFLAGS += -g -ggdb
endif

#######################################
# LDFLAGS
#######################################
# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs $(LIBDIR) $(LIBS) -L$(BSP_PREFIX) -Wl,--gc-sections --verbose

# macros for gcc
# AS defines
AS_DEFS = 

# AS includes
AS_INCLUDES = \
-I$(BSP_PREFIX)/Core/Inc \

# C includes
C_INCLUDES = \
-I$(BSP_PREFIX)/Core/Inc \
-I$(BSP_PREFIX)/Drivers/STM32F0xx_HAL_Driver/Inc \
-I$(BSP_PREFIX)/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy \
-I$(BSP_PREFIX)/Drivers/CMSIS/Device/ST/STM32F0xx/Include \
-I$(BSP_PREFIX)/Drivers/CMSIS/Include

######################################
# source
######################################
# C sources
C_SOURCES += \
$(wildcard $(BSP_PREFIX)/Core/Src/*.c) \
$(wildcard $(BSP_PREFIX)/Drivers/STM32F0xx_HAL_Driver/Src/*.c)

# ASM sources
ASM_SOURCES += \
$(wildcard $(BSP_PREFIX)/*.s)
