SRC:=$(wildcard a*.c)
PPM:=$(patsubst %.c,%.ppm,$(SRC))

#CC=$(CXX)

all: $(PPM)
	md5sum *.ppm

OPT=-O3
#OPT=-O0 -g
#DEBUG:=1
ifdef DEBUG
OPT+=-DDEBUG
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%.elf: %.c
	$(CC) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
