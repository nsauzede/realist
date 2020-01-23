
V:=v

all:

%_v.c: %.v
	$(V) -o $@ $^

%.elf: %_v.c
	$(CC) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *_v.c *.elf *.ppm
