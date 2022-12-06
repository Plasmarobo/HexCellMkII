
TARGET=bootloader
# Pull in BSP variables
include $(BSP_PREFIX)/build.mk

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
LDSCRIPT = ../$(TARGET)/BOOTLOADER.ld
LDFLAGS += -T$(LDSCRIPT) -Wl,-Map=$(TARGET).map,--cref
.PHONY: all

all: $(TARGET).srec $(TARGET).elf $(TARGET).hex $(TARGET).bin

# Pull in general recepies
include ../build.mk