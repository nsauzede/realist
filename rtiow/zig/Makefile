SRC:=$(wildcard main*.zig)
PPM:=$(patsubst %.zig,%.ppm,$(SRC))

ZIG=zig

all: $(PPM)
	md5sum *.ppm

#OPT=-O
#OPT=-O0 -g
#DEBUG:=1
ifdef DEBUG
OPT+=-DDEBUG
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%.elf: %.zig
	$(ZIG) build-exe --name $@ $^ $(OPT)

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.elf.o *.ppm
