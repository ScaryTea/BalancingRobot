INCDIR = libs/include/
OBJDIR = obj/
SRCDIR = libs/source/
LINKDIR = linker/
DEPS =i2c uart mpu6050 motor delay ir

STD ?= c99

f ?= main
FILENAME ?= $(f)
override OBJECTS += $(addsuffix .o, $(DEPS)) $(FILENAME).o startup_stm32f103xb.o system_stm32f1xx.o 

COMP = arm-none-eabi-gcc -Wall --std=$(STD) -g3
COMP += -mthumb -mcpu=cortex-m3 -ffreestanding
COMP += -DDEBUG -DSTM32F103xB -I$(INCDIR) -c 

ELF = arm-none-eabi-gcc -Wall -Wl,-L$(LINKDIR) -TSTM32F103XB_FLASH.ld
ELF += -std=$(STD) --specs=nosys.specs -mthumb -mcpu=cortex-m3
ELF += $(addprefix $(OBJDIR), $(OBJECTS)) -o $(FILENAME).elf

BIN = arm-none-eabi-objcopy -O binary $(FILENAME).elf $(FILENAME).bin

vpath %.h $(INCDIR)
vpath %.o $(OBJDIR)
vpath %.c $(SRCDIR)
vpath %.s $(SRCDIR)
vpath %.ld $(LINKDIR)

.PHONY: all flash clean mkdir

all: clean mkdir $(FILENAME).bin

full: all flash

$(FILENAME).bin: $(FILENAME).elf
	$(BIN)
	@rm -f $(FILENAME).elf

$(FILENAME).elf: $(OBJECTS)
	$(ELF)

%.o : %.c
	$(COMP) $^
	@mv $@ $(OBJDIR)$@

%.o : %.s
	$(COMP) $^
	@mv $@ $(OBJDIR)$@

flash:
	@st-flash write $(FILENAME).bin 0x8000000

clean:
	@rm -f *.bin $(OBJDIR)/$(FILENAME).o

mkdir:
	@mkdir -p $(OBJDIR)

uart:
	socat -,raw,echo=0,escape=0x03 /dev/ttyUSB0,b9600,raw,echo=0

