SRC:=$(wildcard a*.c)
PPM:=$(patsubst %.c,%.ppm,$(SRC))

#CC=$(CXX)

all: $(PPM)
	md5sum *.ppm

%.elf: %.c
	$(CC) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
