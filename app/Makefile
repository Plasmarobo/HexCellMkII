TARGET = app_$(HW_VERSION)
# Pull in BSP variables
include $(BSP_PREFIX)/build.mk

C_INCLUDES += \
-I../common/hal \
-I../common \
-I../app \
-I../Middlewares/Third_Party/FIFO \
-I../Middlewares/Third_Party/FreeRTOS/Source/include \
-I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
-I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0

# Pull in c files from bootloader and common, the BSP should already be defined
C_SOURCES += \
$(wildcard ../app/*.c) \
$(wildcard ../common/*.c) \
$(wildcard ../Middlewares/Third_Party/FIFO/*.c) \
$(wildcard ../Middlewares/Third_Party/FreeRTOS/Source/*.c) \
$(wildcard ../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0/*.c) \
$(wildcard ../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/*.c)

# link script
LDSCRIPT = ../app/APP.ld
LDFLAGS += -T$(LDSCRIPT) -Wl,-Map=$(TARGET).map,--cref
.PHONY: all

all: $(TARGET).srec $(TARGET).elf $(TARGET).hex $(TARGET).bin $(TARGET).fwu

# Pull in general recepies
include ../build.mk