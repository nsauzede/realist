SRC:=$(wildcard main*.c)
EXE:=$(patsubst %.c,%.elf,$(SRC))
PPM:=$(patsubst %.c,%.ppm,$(SRC))

#CC=$(CXX)

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

OPT=$(CFLAGS) -O3
# OPT+=-march=native
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
