TARGET_NAME = app_$(HW_VERSION)

IMAGE_MAGIC = 0xA9911C47
# Pull in BSP variables
include $(BSP_PREFIX)/build.mk

C_DEFS += \
-DFREERTOS

C_INCLUDES += \
-I$(SOURCE_PATH)/common/hal \
-I$(SOURCE_PATH)/common \
-I$(SOURCE_PATH)/app \
-I$(SOURCE_PATH)/Middlewares/Third_Party/FIFO \
-I$(SOURCE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/include \
-I$(SOURCE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
-I$(SOURCE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/portable/$(FREERTOS_PORTABLE)

# Pull in c files from bootloader and common, the BSP should already be defined
C_SOURCES += \
$(wildcard $(SOURCE_PATH)/app/*.c) \
$(wildcard $(SOURCE_PATH)/common/*.c) \
$(wildcard $(SOURCE_PATH)/Middlewares/Third_Party/FIFO/*.c) \
$(wildcard $(SOURCE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/*.c) \
$(wildcard $(SOURCE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/portable/$(FREERTOS_PORTABLE)/*.c) \
$(wildcard $(SOURCE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/*.c)

# link script
LDSCRIPT = ../app/APP.ld
LDFLAGS += -T$(LDSCRIPT) -Wl,-Map=$(TARGET_NAME).map,--cref
.PHONY: all

all: $(TARGET_NAME).srec $(TARGET_NAME).elf $(TARGET_NAME).hex $(TARGET_NAME).bin $(TARGET_NAME).fwu

# Pull in general recepies
include ../build.mk