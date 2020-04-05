SRC:=$(wildcard main*.odin)
PPM:=$(patsubst %.odin,%.ppm,$(SRC))

ODIN=odin

all: $(PPM)
	md5sum *.ppm

#OPT=-llvm-api
#OPT=-opt=3
#OPT=-O0 -g
#DEBUG:=1
ifdef DEBUG
OPT+=-define:DEBUG=1
endif
#OPT+=-debug

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%.elf: %.odin
	$(ODIN) build $^ -out:$@ $(OPT)

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
