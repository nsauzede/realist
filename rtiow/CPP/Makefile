SRC:=$(wildcard *.cpp)
PPM:=$(patsubst %.cpp,%.ppm,$(SRC))

all: $(PPM)
	md5sum *.ppm

%.elf: %.cpp
	$(CXX) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *.elf *.ppm
