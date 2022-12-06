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

%.elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

%.hex: %.elf
	$(HEX) $< $@
	
%.bin: %.elf
	$(BIN) $< $@	
	
%.srec: %.elf
	$(CP) -O srec $< $@

$(BUILD_DIR):
	mkdir -p $@

#######################################
# dependencies
#######################################
-include $(wildcard *.d)