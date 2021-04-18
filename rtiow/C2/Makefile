SRC:=$(wildcard main*.c)
EXE:=$(patsubst %.c,%.elf,$(SRC))
PPM:=$(patsubst %.c,%.ppm,$(SRC))

#CC=$(CXX)

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

bench: main14.elf
	/usr/bin/time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm

# OPT=$(CFLAGS) -O3
# OPT+=-march=native
#DEBUG:=1
ifdef DEBUG
OPT:=-O0 -g
OPT+=-DDEBUG
else
OPT:=-O3
# OPT:=-Ofast				# ==> this one breaks bit-exact non-reg
OPT+=-fno-plt
OPT+=-flto
# OPT+=-march=native	# ==> this one breaks bit-exact non-reg
OPT+=-DNDEBUG
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
