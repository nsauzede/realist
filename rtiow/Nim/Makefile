SRC:=$(wildcard main*.nim)
EXE:=$(patsubst %.nim,%.elf,$(SRC))
PPM:=$(patsubst %.nim,%.ppm,$(SRC))

NIM=nim

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

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

NIMOPT:=
NIMOPT+=--hints:off
#NIMOPT+=-d:release
#NIMOPT+=-d:danger
#NIMOPT+=--gc:orc

%.elf: %.nim
	$(NIM) c $(NIMOPT) -o:$@ $^ $(OPT)

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.elf.o *.ppm
