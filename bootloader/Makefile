
TARGET_NAME = bootloader_$(HW_VERSION)

FEATURES = RCC DMA FLASH TIM UART GPIO CORTEX CRC
$(info FEATURESET: $(FEATURES))
# Pull in BSP variables
include $(SOURCE_ROOT)/$(BSP_PREFIX)/build.mk

# Jump-to address
BOOT_ADDRESS = 0x8006800

# Should be relative to SOURCE_ROOT
INCLUDE_DIRS += \
common/hal \
common \
Middlewares/Third_Party/FIFO

# Pull in c files from bootloader and common, the BSP should already be defined
SOURCE_DIRS += \
bootloader \
common \
Middlewares/Third_Party/FIFO

# link script
LDSCRIPT = $(SOURCE_ROOT)/$(TARGET)/BOOTLOADER.ld
LDFLAGS += -T$(LDSCRIPT) -Wl,-Map=$(TARGET_NAME).map,--cref

# Pull in general recepies
include $(SOURCE_ROOT)/build.mk