all:

%.elf: %.cpp
	$(CXX) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *.elf *.ppm
