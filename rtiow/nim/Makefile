SRC:=$(wildcard main*.nim)
PPM:=$(patsubst %.nim,%.ppm,$(SRC))

NIM=nim

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

%.elf: %.nim
	$(NIM) build-exe --name $@ $^ $(OPT)

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.elf.o *.ppm
