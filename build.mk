# Contains generic recepies to costruct C files
# SOURCES and ASM_SOURCES must be populated by the caller
# BUILD_DIR is expected to be set, CFLAGS and LDFLAGS should be set as well

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
ASFLAGS = $(CFLAGS)
#######################################
# build the application
#######################################
# list of objects

C_SOURCES = $(sort $(foreach dir,$(SOURCE_DIRS),$(wildcard $(SOURCE_ROOT)/$(dir)/*.c)))
OBJECTS = $(subst $(SOURCE_ROOT)/,,$(C_SOURCES:%.c=%.o))
vpath %.c $(sort $(foreach sdir,$(SOURCE_DIRS),$(SOURCE_ROOT)/$(dir sdir)))
# list of ASM program objects
ASM_SOURCES = $(sort $(foreach dir,$(SOURCE_DIRS),$(wildcard $(SOURCE_ROOT)/$(dir)/*.s)))
OBJECTS += $(subst $(SOURCE_ROOT)/,,$(ASM_SOURCES:%.s=%.o))
vpath %.s $(sort $(foreach sdir,$(SOURCE_DIRS),$(SOURCE_ROOT)/$(dir sdir)))

C_INCLUDES = $(foreach idir,$(INCLUDE_DIRS),-I$(SOURCE_ROOT)/$(idir))
AS_INCLUDES = $(foreach idir,$(INCLUDE_DIRS),-I$(SOURCE_ROOT)/$(idir))

DEPENDENCIES = $(subst $(SOURCE_ROOT)/,,$(C_SOURCES:%.c=%.d))

$(info ================================= Sources =================================)
$(foreach f,$(C_SOURCES),$(info $(f)))
$(foreach f,$(ASM_SOURCES),$(info $(f)))
$(info ================================= Objects =================================)
$(foreach o,$(OBJECTS),$(info $(o)))
$(info =================================  Prep  ==================================)
$(shell mkdir -p $(dir $(OBJECTS)) >/dev/null)
$(info =================================  Build  =================================)

all: $(TARGET_NAME).elf $(TARGET_NAME).hex $(TARGET_NAME).bin $(TARGET_NAME).fwu $(TARGET_NAME).srec 

%.o: $(SOURCE_ROOT)/%.c
	$(MKDIR) $(@D)
	$(CC) $(CFLAGS) -Wa,-a,-ad,-alms=$(notdir $(<:.c=.lst)) $< -o $@

%.o: $(SOURCE_ROOT)/%.s
	$(MKDIR) $(@D)
	$(AS) $(CFLAGS) $< -o $@

$(TARGET_NAME).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(TARGET_NAME).hex: $(TARGET_NAME).elf
	$(HEX) $< $@
	
$(TARGET_NAME).bin: $(TARGET_NAME).elf
	$(BIN) $< $@	
	
$(TARGET_NAME).srec: $(TARGET_NAME).elf
	$(CP) -O srec $< $@

$(TARGET_NAME).fwu: $(TARGET_NAME).bin
	python $(SOURCE_ROOT)/tools/image_builder.py -f $< -v $(FW_VERSION_MAJOR).$(FW_VERSION_MINOR).$(FW_VERSION_PATCH) -r $(HW_VERSION) -o $@

#######################################
# dependencies
#######################################
-include $(DEPENDENCIES)