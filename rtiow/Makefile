TARGET:=
TARGET+=C/main14.elf
TARGET+=CPP/main14.elf
TARGET+=Odin/main14.elf
TARGET+=Rust/main14.elf
#TARGET+=V/A14_v
#TARGET+=V/A14_v.c

all: $(TARGET)

CPP/main14.elf:
	(cd CPP; make clean ; /usr/bin/time make main14.elf)

C/main14.elf:
	(cd C; make clean ; /usr/bin/time make main14.elf)

OOPT:=
OOPT+=-opt:3
OOPT+=-no-bounds-check
Odin/main14.elf:
	(cd Odin; make clean ; /usr/bin/time make main14.elf OPT="$(OOPT)")

Rust/main14.elf:
	(cd Rust; make clean ; /usr/bin/time make main14.elf)

V/A14_v: V/A14_v.c
	(cd V; gcc -o A14_v A14_v.c -O3 -lm)

V/A14_v.c: V/A14.v
	(cd V; v -o A14_v.c A14.v)

bench: $(TARGET)
	/usr/bin/time C/main14.elf 1024 768 10 main14.ppm
	/usr/bin/time CPP/main14.elf 1024 768 10 main14.ppm
	/usr/bin/time Odin/main14.elf 1024 768 10 main14.ppm
	/usr/bin/time Rust/main14.elf 1024 768 10 main14.ppm
#	/usr/bin/time V/A14_v 200 100 100 A14.ppm

clean:
	$(RM) $(TARGET)

clobber: clean
	$(RM) *_v.c

mrproper: clobber
	$(RM) *.ppm
	$(MAKE) -C C clean
	$(MAKE) -C V clean
	$(MAKE) -C CPP clean
	$(MAKE) -C Rust clean
	$(MAKE) -C Go clean
	$(MAKE) -C Odin clean
