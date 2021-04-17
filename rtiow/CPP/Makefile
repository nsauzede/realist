SRC:=$(wildcard main*.cpp)
EXE:=$(patsubst %.cpp,%.elf,$(SRC))
PPM:=$(patsubst %.cpp,%.ppm,$(SRC))

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

bench: main14.elf
	/usr/bin/time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm

ifdef DEBUG
OPT:=-O0 -g
OPT+=-DDEBUG
else
# OPT:=-O3
# OPT:=-Ofast -fno-plt -flto -march=native -DNDEBUG
# OPT:=-Ofast -fno-plt -flto -DNDEBUG
OPT:=-O3 -fno-plt -flto -DNDEBUG
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%.elf: %.cpp
	$(CXX) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
