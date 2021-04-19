SRC:=$(wildcard main*.nelua)
EXE:=$(patsubst %.nelua,%.elf,$(SRC))
PPM:=$(patsubst %.nelua,%.ppm,$(SRC))

NELUA:=nelua

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

bench: main14.elf
	time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm

#DEBUG:=1
NFLAGS+=-q --no-cache --cache-dir . -c
ifdef DEBUG
OPT:=-O0 -g
#NFLAGS+=
else
# OPT:=-O3
# OPT:=-Ofast -fno-plt -flto -march=native -DNDEBUG
OPT:=-O3 -fno-plt -flto -DNDEBUG
# OPT:=-Ofast -fno-plt -flto -DNDEBUG
# NFLAGS+=-Pnochecks -Pnogc # ==> gcc: 44s instead of 38s, clang: 33s instead of 35s
# NFLAGS+=-Pnochecks		# ==> gcc: 46s instead of 38s, clang: 34s instead of 35s
NFLAGS+=-Pnogc			# ==> gcc: 36s instead of 38s, clang: 34s instead of 35s
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%.c: %.nelua
	$(NELUA) $(NFLAGS) -o $@ $^

# main14.elf: main14.nelua
# #	$(NELUA) --maximum-performance -b -o $@ $^
# 	$(NELUA) --release -Pnochecks -Pnogc -b -o $@ $^

%.elf: %.c
	$(CC) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *.c *.elf *.ppm
