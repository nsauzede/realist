SRC:=$(wildcard main*.nim)
EXE:=$(patsubst %.nim,%.elf,$(SRC))
PPM:=$(patsubst %.nim,%.ppm,$(SRC))

NIM=nim

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

bench: main14.elf
	time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm

NIMOPT:=
NIMOPT+=--hints:off

#DEBUG:=1
ifdef DEBUG
NIMOPT+=--debugger:native
endif

#PROF:=1
ifdef PROF
NIMOPT+=--profiler:on --stackTrace:on
endif

OPT:=1
ifdef OPT
NIMOPT+=-d:release
NIMOPT+=-d:danger
#NIMOPT+=--gc:orc
# NIMOPT+=--passC:"-O3"
# NIMOPT+=--passC:"-Ofast -fno-plt -flto -march=native -DNDEBUG"
# NIMOPT+=--passC:"-Ofast -fno-plt -flto -DNDEBUG"
NIMOPT+=--passC:"-O3 -fno-plt -flto -DNDEBUG"
NIMOPT+=--gc:arc
NIMOPT+=-d:lto
endif

%.elf: %.nim
	$(NIM) c $(NIMOPT) -o:$@ $^

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.elf.o *.ppm
