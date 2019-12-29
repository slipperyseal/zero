# zero - pre-emptive multitasking kernel for AVR
#
#  Techno Cosmic Research Institute		Dirk Mahoney			dirk@tcri.com.au
#  Catchpole Robotics					Christian Catchpole		christian@catchpole.net


#######################################################################################
########################## DEVELOPER-ADJUSTABLE SETTINGS HERE #########################
#######################################################################################


# output file and project name
OUTPUT = zero

# flashing settings
AVRDUDE_PART = m1284p
AVRDUDE_CFG = pi

# general kernel settings
F_CPU = 20'000'000
QUANTUM_TICKS = 15
PAGE_BYTES = 16

# some standard MCU settings
LFUSE = 0xFF
HFUSE = 0xD9
EFUSE = 0xFF


ifeq ($(AVRDUDE_PART),m328p)
	# 1.5KB for allocator
	MCU = atmega328p
	DYNAMIC_BYTES = 1536
endif

ifeq ($(AVRDUDE_PART),m644p)
	# 3KB for allocator
	MCU = atmega644p
	DYNAMIC_BYTES = 3172
endif

ifeq ($(AVRDUDE_PART),m1284p)
	# 15KB for allocator
	MCU = atmega1284p
	DYNAMIC_BYTES = 15360
endif


#######################################################################################
######################### END OF DEVELOPER-ADJUSTABLE SETTINGS ########################
#######################################################################################


PC_COUNT = 2


# probably don't adjust these so much :)
FLAGS += -g
FLAGS += -Os
FLAGS += --std=c++17
FLAGS += -mmcu=$(MCU)
FLAGS += -fshort-enums

# #define pass-throughs
FLAGS += -DF_CPU=$(F_CPU)ULL
FLAGS += -DPC_COUNT=$(PC_COUNT)
FLAGS += -DPAGE_BYTES=$(PAGE_BYTES)
FLAGS += -DDYNAMIC_BYTES=$(DYNAMIC_BYTES)
FLAGS += -DPROJ_NAME=\"$(OUTPUT)\"
FLAGS += -DQUANTUM_TICKS=$(QUANTUM_TICKS)


CC = avr-gcc
OBJ := $(patsubst %.cpp,%.o,$(wildcard *.cpp))


.PHONY: push fuses upload clean gettools


%.o: %.cpp
	@echo -n "Compiling $^..."
	@$(CC) $(FLAGS) -c $^
	@echo " done"


$(OUTPUT).elf: $(OBJ)
	@echo -n "Linking..."
	@$(CC) $(FLAGS) $(LDFLAGS) -o $@ $^
	@echo " done"
	@avr-size -A -x --mcu=$(MCU) $@
	@avr-size -C -x --mcu=$(MCU) $@


upload: $(OUTPUT).elf
	@sudo avrdude -p $(AVRDUDE_PART) -c $(AVRDUDE_CFG) -U flash:w:$(OUTPUT).elf


fuses:
	@sudo avrdude -p $(AVRDUDE_PART) -c $(AVRDUDE_CFG) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m


push: $(OUTPUT).elf
	@echo -n "Pushing files to RPi..."
	@rsync -avr --exclude '.git' ~/dev/$(OUTPUT)/ pi:dev/$(OUTPUT)/ > /dev/null
	@rm -f *.o *.elf *.hex
	@echo " done"


clean:
	@echo -n "Cleaning up..."
	@rm -f *.o *.elf *.hex
	@echo " done"


gettools:
	@sudo apt-get -y install gcc-avr binutils-avr gdb-avr avr-libc avrdude
