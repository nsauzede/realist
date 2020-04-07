SRC:=$(wildcard main*.odin)
PPM:=$(patsubst %.odin,%.ppm,$(SRC))

_SYS:=$(shell uname -o)
ifeq ($(_SYS),Msys)
WIN32:=1
EXEXT:=exe
ODIN=odin.exe
else
EXEXT:=elf
ODIN=odin
endif

all: $(PPM)
	md5sum *.ppm

#OPT=-llvm-api
#OPT=-opt:3
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

%.$(EXEXT): %.odin pcg/pcg.odin
	$(ODIN) build $< -out:$@ $(OPT)

%.ppm: %.$(EXEXT)
	./$^ $(ARGS) > $@

clean:
	$(RM) *.$(EXEXT) *.ppm
