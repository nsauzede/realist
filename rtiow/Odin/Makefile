SRC:=$(wildcard main*.odin)
EXE:=$(patsubst %.odin,%.elf,$(SRC))
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

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

#OPT=-llvm-api
OPT=-opt:3 -no-bounds-check
#OPT=-O0 -g
#DEBUG:=1
ifdef DEBUG
OPT+=-define:DEBUG=1
endif
#OPT+=-debug

%.$(EXEXT): %.odin pcg/pcg.odin
	$(ODIN) build $< -out:$@ $(OPT)

%.ppm: %.$(EXEXT)
	./$^ $(ARGS) > $@

clean:
	$(RM) *.$(EXEXT) *.ppm
