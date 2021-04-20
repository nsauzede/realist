SRC:=$(wildcard main*.go)
EXE:=$(patsubst %.go,%.elf,$(SRC))
PPM:=$(patsubst %.go,%.ppm,$(SRC))

GO=go

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

bench: main14.elf
	time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm

#OPT:=--ldflags '-extldflags "-lm"'
#DEBUG:=1
ifdef DEBUG
OPT+=-DDEBUG
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%.elf: %.go src/pcg/pcg.go
	$(GO) build $(OPT) -o $@ $<

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
