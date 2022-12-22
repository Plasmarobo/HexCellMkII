
TARGET_NAME = bootloader_$(HW_VERSION)
IMAGE_MAGIC = 0xB007104D

# Pull in BSP variables
include $(BSP_PREFIX)/build.mk

# Jump-to address
BOOT_ADDRESS = 0x8006800

C_INCLUDES += \
-I../common/hal \
-I../common \
-I../Middlewares/Third_Party/FIFO

# Pull in c files from bootloader and common, the BSP should already be defined
C_SOURCES += \
$(wildcard ../bootloader/*.c) \
$(wildcard ../common/*.c) \
$(wildcard ../Middlewares/Third_Party/FIFO/*.c)

# link script
LDSCRIPT = ../bootloader/BOOTLOADER.ld
LDFLAGS += -T$(LDSCRIPT) -Wl,-Map=$(TARGET_NAME).map,--cref
.PHONY: all

all: $(TARGET_NAME).elf $(TARGET_NAME).hex $(TARGET_NAME).bin $(TARGET_NAME).fwu $(TARGET_NAME).srec 

# Pull in general recepies
include ../build.mk