################################################################################
# Simple makefile for the led example
################################################################################

RM := rm -rf

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS = test.c

OBJS = test.o
LIBS = -lm

SIZEDUMMY = sizedummy

# All Target
all: test.hex

# Tool invocations
%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -fwhole-program -ffreestanding -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

test.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -fwhole-program -ffreestanding -mmcu=atmega2560 -o"test.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

test.hex: test.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex test.elf  "test.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo "Creating extended listing file" $@
	avr-objdump -h -S -z $< > $@

sizedummy: test.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=berkeley -t test.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS) test.elf test.hex test.d
	-@echo ' '

upload: test.hex
	@echo 'Invoking: Avrdude'
	avrdude -c stk500v2 -p m2560 -P /dev/ttyACM0 -U flash:w:test.hex
	@echo 'Finished uploading: $@'
	@echo ' '

.PHONY: all clean dependents
.SECONDARY:
