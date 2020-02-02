SRC:=$(wildcard a*.c)
PPM:=$(patsubst %.c,%.ppm,$(SRC))

#CC=$(CXX)

all: $(PPM)
	md5sum *.ppm

OPT=-O3
#OPT=-O0 -g
#OPT+=-DDEBUG
%.elf: %.c
	$(CC) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
