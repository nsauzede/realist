SRC:=$(wildcard a*.c)
PPM:=$(patsubst %.c,%.ppm,$(SRC))

#CC=$(CXX)

all: $(PPM)
	md5sum *.ppm

OPT=-O3
#OPT=-O0 -g
ifdef DEBUG
OPT+=-DDEBUG
endif
%.elf: %.c
	$(CC) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
