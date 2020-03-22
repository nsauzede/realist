SRC:=$(wildcard main*.go)
PPM:=$(patsubst %.go,%.ppm,$(SRC))

GO=go

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

%.elf: %.go
	$(GO) build -o $@ $^ $(OPT)

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
