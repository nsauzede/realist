SRC:=$(wildcard main*.cpp)
PPM:=$(patsubst %.cpp,%.ppm,$(SRC))

all: $(PPM)
	md5sum *.ppm

%.elf: %.cpp
	$(CXX) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
