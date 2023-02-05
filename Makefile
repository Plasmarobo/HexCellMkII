##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.17.1] date: [Tue Dec 20 18:22:11 EST 2022] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# building variables
######################################
include versions.mk

# debug build?
DEBUG ?= 1
# optimization
OPT ?= -Og

BSP ?= stm32f103c8tx
SOURCE_ROOT = ..
BSP_PREFIX = bsp/$(BSP)
#######################################
# paths
#######################################
# Build path
TARGET ?= bootloader
BUILD_DIR ?= build
OUTPUT_DIR ?= out
HW_VERSION ?= $(HW_VERSION_MKVR1L)
FW_VERSION_MAJOR ?= 0
FW_VERSION_MINOR ?= 1
FW_VERSION_PATCH ?= 0
MKDIR = mkdir -p
export

.PHONY: all clean $(TARGET)

# default action: build all
all: $(TARGET)

$(TARGET):
	$(MKDIR) $(BUILD_DIR)
	cd $(BUILD_DIR); $(MAKE) -f $(SOURCE_ROOT)/$(TARGET)/Makefile

clean:
	$(RM) -fR $(BUILD_DIR)

# *** EOF ***
