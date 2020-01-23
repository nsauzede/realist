all:

%.elf: %.c
	$(CC) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *.elf *.ppm
