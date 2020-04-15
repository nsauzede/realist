SRC:=$(wildcard main*.cpp)
EXE:=$(patsubst %.cpp,%.elf,$(SRC))
PPM:=$(patsubst %.cpp,%.ppm,$(SRC))

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

OPT=$(CXXFLAGS) -O3
#OPT=-O0 -g
ifdef DEBUG
OPT+=-DDEBUG
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
