SRC:=$(wildcard main*.cpp)
PPM:=$(patsubst %.cpp,%.ppm,$(SRC))

all: $(PPM)
	md5sum *.ppm

OPT=-O3
#OPT=-O0 -g
%.elf: %.cpp
	$(CXX) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
