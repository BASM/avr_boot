# Name: Makefile

#DEVICE = avrt2313
#DEVICE = at90s2313
#DEVICE = attiny15
#DEVICE = atmega168
#DEVICE = atmega328p
DEVICE = atmega328p
#DEVICE = atmega88
#DEVICE = atmega168
#TYPEPROG=siprog -P /dev/ttyS1
#TYPEPROG=siprog -P /dev/ttyUSB0
#TYPEPROG=stk500v2 -P avrdoper
TYPEPROG=dragon_pp
#TYPEPROG=stk500v2 -P /dev/ttyACM2
AVRDUDE = avrdude -c ${TYPEPROG} -p m328p
#AVRDUDE = avrdude -c ${TYPEPROG} -p m168
#AVRDUDE = avrdude  -p m88 -c stk500v2 -P avrdoper

#COMPILE = avr-gcc -save-temps -Wall -Os -Iusbdrv -I. -mmcu=$(DEVICE)  -L/usr/i686-pc-linux-gnu/avr/lib #-DDEBUG_LEVEL=2
#COMPILE = avr-gcc -save-temps -Wall -Os -Iusbdrv -I. -mmcu=$(DEVICE)  -L/usr/x86_64-pc-linux-gnu/avr/lib #-DDEBUG_LEVEL=2
#COMPILE = avr-gcc -save-temps -Os -Wall -Iusbdrv -I. -mmcu=$(DEVICE)  
# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.

#BOOTSTART=0x3800
BOOTSTART=0x7000
LDLIBS+=-Wl,--section-start=.text=$(BOOTSTART) 


#OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o usbtty.o
OBJECTS = main.o

DEPLIBS+=libavr
AVRLIBS+=uart
AVRLIBS+=stdioconf

AVR_LDLIBS=-Llib/libavr $(foreach lib,${AVRLIBS},-l ${lib}) 
AVR_CFLAGS=-Ilib/libavr/include
AVR_DEPS=$(foreach lib,${LIBS},lib/libavr/lib${lib}.a)


LDLIBS+=${AVR_LDLIBS}
CFLAGS+=${AVR_CFLAGS}
DEPS  +=${AVR_DEPS}

#LIBS=ir
DEPLIBS+=$(LIBS:%=lib%)
LDLIBS+=$(foreach lib,${LIBS},-Llib/lib${lib} -l${lib})
CFLAGS+=$(foreach inc,${LIBS},-Ilib/lib${inc}/include)
DEPS=$(foreach lib,${LIBS},lib/lib${lib}/lib${lib}.a)


#COMPILE = avr-gcc -flto -Os -Wall -I. -mmcu=$(DEVICE) -DF_CPU=${FREQ} $(CFLAGS)
COMPILE = avr-gcc  -Os -Wall -I. -mmcu=$(DEVICE) -DF_CPU=${FREQ} $(CFLAGS)
# symbolic targets:
all: $(DEPLIBS) main.hex

${DEPLIBS}:
	make -C lib/$@/
#libir:
#	make -C libs/libir/

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -V -U flash:w:main.hex:i

read:	all
	$(AVRDUDE) -U flash:r:read.hex:i
	objcopy -I ihex -O ihex read.hex read16.hex

# Fuse low byte:
# 0xef = 1 1 1 0   1 1 1 1
#        ^ ^ \+/   \--+--/
#        | |  |       +------- CKSEL 3..0 (clock selection -> crystal @ 12 MHz)
#        | |  +--------------- SUT 1..0 (BOD enabled, fast rising power)
#        | +------------------ CKOUT (clock output on CKOUT pin -> disabled)
#        +-------------------- CKDIV8 (divide clock by 8 -> don't divide)
#
# Fuse high byte:
# 0xdb = 1 1 0 1   1 0 1 1
#        ^ ^ ^ ^   \-+-/ ^
#        | | | |     |   +---- RSTDISBL (disable external reset -> enabled)
#        | | | |     +-------- BODLEVEL 2..0 (brownout trigger level -> 2.7V)
#        | | | +-------------- WDTON (watchdog timer always on -> disable)
#        | | +---------------- SPIEN (enable serial programming -> enabled)
#        | +------------------ EESAVE (preserve EEPROM on Chip Erase -> not preserved)
#        +-------------------- DWEN (debug wire enable)
# 0x5b = 0 1 0 1   1 0 1 1
fuse_tiny2313:	# only needed for attiny2313
	$(AVRDUDE) -U hfuse:w:0xdb:m -U lfuse:w:0xef:m

# Fuse low byte:
# 0xef = 1 1 1 0   1 1 1 1 (at2313)
# 0xe7   1 1 1 0   0 1 1 1 
#        ^ ^ \+/   \--+--/
#        | |  |       +------- CKSEL 3..0 (clock selection -> crystal @ 12 MHz, FULL swing)
#        | |  +--------------- SUT 1..0 (BOD enabled, fast rising power)
#        | +------------------ CKOUT (clock output on CKOUT pin -> disabled)
#        +-------------------- CKDIV8 (divide clock by 8 -> don't divide)
#
# Fuse high byte:
# 0xda = 1 1 0 1   1 0 1 0
# 0xdb = 1 1 0 1   1 0 1 1
# 0xd8   1 1 0 1   1 0 0 0
# 0xdf   1 1 0 1   1 1 1 1
#        ^ ^ ^ ^   | | / ^
#        | | | |   | |/  +---- Select Reset Vector (0-BOOT,1-APP)
#        | | | |   | +-------- BootSZ (00 big, 11 small)
#        | | | |   \---------- (not, it at2313) 
#        | | | +-------------- (not, it at2313) WDTON (watchdog timer always on -> disable)
#        | | +---------------- (not, it at2313) SPIEN (enable serial programming -> enabled)
#        | +------------------ (not, it at2313) EESAVE (preserve EEPROM on Chip Erase -> not preserved)
#        +-------------------- (not, it at2313) DWEN (debug wire enable)
# 0x5b = 0 1 0 1   1 0 1 1

fuse_mega88:	# only needed for atmega88
	$(AVRDUDE) -U hfuse:w:0xdf:m -U lfuse:w:0xe7:m

fuse_mega168:	# only needed for atmega168
	$(AVRDUDE) -U hfuse:w:0xdf:m -U lfuse:w:0xe7:m

## USE external 12 mhz, in BOOT mode
fuse_mega328P:	# only needed for atmega328P
	$(AVRDUDE) -U hfuse:w:0xd8:m -U lfuse:w:0xe7:m

fuse_mega328P_8i:
	  $(AVRDUDE) -U hfuse:w:0xd8:m -U lfuse:w:0xa2:m

clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.bin *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s

main.bin:	$(OBJECTS) $(DEPS)
	$(COMPILE) -o main.bin $(OBJECTS) $(LDLIBS) 

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	./checksize main.bin
# do the checksize script as our last action to allow successful compilation
# on Windows with WinAVR where the Unix commands will fail.

disasm:	main.bin
	avr-objdump -Sd main.bin

cpp:
	$(COMPILE) -E main.c
