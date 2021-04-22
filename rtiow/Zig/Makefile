SRC:=$(wildcard main*.zig)
EXE:=$(patsubst %.zig,%.elf,$(SRC))
PPM:=$(patsubst %.zig,%.ppm,$(SRC))

ZIG=zig

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

bench: main14.elf
	time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm

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
