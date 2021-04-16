SRC:=$(wildcard main*.cpp)
EXE:=$(patsubst %.cpp,%.elf,$(SRC))
PPM:=$(patsubst %.cpp,%.ppm,$(SRC))

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

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

%.elf: %.cpp
	$(CXX) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
