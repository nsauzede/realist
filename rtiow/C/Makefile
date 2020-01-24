SRC:=$(wildcard *.c)
PPM:=$(patsubst %.c,%.ppm,$(SRC))

all: $(PPM)

%.elf: %.c
	$(CC) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *.elf *.ppm
