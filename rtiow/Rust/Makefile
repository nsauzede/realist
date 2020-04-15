SRC:=$(wildcard main*.rs)
EXE:=$(patsubst %.rs,%.elf,$(SRC))
PPM:=$(patsubst %.rs,%.ppm,$(SRC))

RSC=rustc

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

OPT=-C opt-level=3
#DEBUG:=1
ifdef DEBUG
OPT+=--cfg 'DEBUG'
endif

%.elf: %.rs pcg.rs
	$(RSC) -o $@ $< $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
