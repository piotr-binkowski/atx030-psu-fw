CC=avr-gcc
CFLAGS=-Os -DF_CPU=1000000UL -mmcu=attiny2313a
OBJCOPY=avr-objcopy

all: psu.hex

program: psu.hex
	minipro -p attiny2313a -c code -f ihex -w $<

clean:
	rm -f psu.hex psu.elf psu.o

psu.hex: psu.elf
	${OBJCOPY} -O ihex $< $@

psu.elf: psu.o
	${CC} ${CFLAGS} -o $@ $^

.PHONY: program clean all
