# Contains generic recepies to costruct C files
# SOURCES and ASM_SOURCES must be populated by the caller
# BUILD_DIR is expected to be set, CFLAGS and LDFLAGS should be set as well

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(notdir $(C_SOURCES:.c=.o))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(notdir $(ASM_SOURCES:.s=.o))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(info ================================= Sources =================================)
$(foreach f,$(C_SOURCES),$(info $(f)))
$(foreach f,$(ASM_SOURCES),$(info $(f)))
$(info ================================= Objects =================================)
$(foreach o,$(OBJECTS),$(info $(o)))
$(info =================================  Build  =================================)

%.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(notdir $(<:.c=.lst)) $< -o $@

$(TARGET_NAME).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(TARGET_NAME).hex: $(TARGET_NAME).elf
	$(HEX) $< $@
	
$(TARGET_NAME).bin: $(TARGET_NAME).elf
	$(BIN) $< $@	
	
$(TARGET_NAME).srec: $(TARGET_NAME).elf
	$(CP) -O srec $< $@

$(TARGET_NAME).fwu: $(TARGET_NAME).bin
	python ../tools/image_builder.py -f $< -v $(FW_VERSION_MAJOR).$(FW_VERSION_MINOR).$(FW_VERSION_PATCH) -r $(HW_VERSION) -o $@

$(BUILD_DIR):
	mkdir -p $@

#######################################
# dependencies
#######################################
-include $(wildcard *.d)