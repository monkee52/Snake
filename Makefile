
DEBUG=1

INCLUDE=-I../cab202_teensy -I../usb_serial
LIBS=-L../cab202_teensy

CFLAGS=-mmcu=atmega32u4 -Os -DF_CPU=16000000UL -std=gnu99 -Wall -Werror
LFLAGS=-mmcu=atmega32u4 -Os -Wl,-u-vfprintf -lprintf_flt -lcab202_teensy -lm

ADDITIONAL_DEPS=

ifeq ($(DEBUG),1)
	CFLAGS+= -DDEBUG
	LFLAGS+= ../usb_serial/usb_serial.o
	ADDITIONAL_DEPS+= slip.o stcp.o stcp_util.o
endif

all: snake

snake: io.o lcd.o screen.o graphics.o rng.o snake.o $(ADDITIONAL_DEPS)
	avr-gcc $(LFLAGS) $(LIBS) io.o lcd.o screen.o graphics.o rng.o snake.o $(ADDITIONAL_DEPS) -o snake.out.o
	avr-objcopy -O ihex snake.out.o snake.hex

io.o:
	avr-gcc -c io.c $(CFLAGS) $(INCLUDE) -o io.o

lcd.o:
	avr-gcc -c lcd.c $(CFLAGS) $(INCLUDE) -o lcd.o

screen.o:
	avr-gcc -c screen.c $(CFLAGS) $(INCLUDE) -o screen.o

graphics.o:
	avr-gcc -c graphics.c $(CFLAGS) $(INCLUDE) -o graphics.o

rng.o:
	avr-gcc -c rng.c $(CFLAGS) $(INCLUDE) -o rng.o

snake.o:
	avr-gcc -c snake.c $(CFLAGS) $(INCLUDE) -o snake.o

slip.o:
	avr-gcc -c slip.c $(CFLAGS) $(INCLUDE) -o slip.o

stcp.o:
	avr-gcc -c stcp.c $(CFLAGS) $(INCLUDE) -o stcp.o

stcp_util.o:
	avr-gcc -c stcp_util.c $(CFLAGS) $(INCLUDE) -o stcp_util.o

clean:
	rm *.o
	rm *.hex
