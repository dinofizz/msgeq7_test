CC=avr-gcc

OBJCOPY=avr-objcopy

CFLAGS=-g -mmcu=atmega644 -std=c99

#Info til AVRDUDE
PROGRAMMER = avrdude
BOARD = stk500
MCU = m644
PORT = /dev/ttyUSB0

PFLAGS = -p $(MCU) -c $(BOARD) -P $(PORT)

rom.hex : msgeq7_test.out
	$(OBJCOPY) -j .text -O ihex msgeq7_test.out rom.hex

msgeq7_test.out : msgeq7_test.o
	$(CC) $(CFLAGS) -o msgeq7_test.out -Wl,-Map,msgeq7_test.map msgeq7_test.o

msgeq7_test.o : msgeq7_test.c
	$(CC) $(CFLAGS) -Os -c msgeq7_test.c

clean:
	rm -f *.o *.out *.map *.hex

program:
	$(PROGRAMMER) $(PFLAGS) -e
	$(PROGRAMMER) $(PFLAGS) -U flash:w:rom.hex
