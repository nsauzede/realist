SRC:=$(wildcard main*.c)
EXE:=$(patsubst %.c,%.elf,$(SRC))
PPM:=$(patsubst %.c,%.ppm,$(SRC))

#CC=$(CXX)

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

# OPT=$(CFLAGS) -O3
# OPT+=-march=native
#DEBUG:=1
ifdef DEBUG
OPT:=-O0 -g
OPT+=-DDEBUG
else
OPT:=-O3
# OPT:=-Ofast -fno-plt -flto -march=native -DNDEBUG
# OPT:=-Ofast -fno-plt -flto -DNDEBUG
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%.elf: %.c
	$(CC) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
