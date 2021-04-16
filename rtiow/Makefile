TARGET:=
TARGET+=Rust/main14.elf
TARGET+=Nelua/main14.elf
TARGET+=Nim/main14.elf
TARGET+=C/main14.elf
TARGET+=CPP/main14.elf
TARGET+=Odin/main14.elf
TARGET+=V/main14.elf
TARGET+=Go/main14.elf

all: $(TARGET)

Rust/main14.elf:
	(cd Rust; make clean ; /usr/bin/time make main14.elf) 

Nelua/main14.elf:
	(cd Nelua; make clean ; /usr/bin/time make main14.elf)

Nim/main14.elf:
	(cd Nim; make clean ; /usr/bin/time make main14.elf)

C/main14.elf:
	(cd C; make clean ; /usr/bin/time make main14.elf)

CPP/main14.elf:
	(cd CPP; make clean ; /usr/bin/time make main14.elf)

Go/main14.elf:
	(cd Go; make clean ; /usr/bin/time make main14.elf)

Odin/main14.elf:
	(cd Odin; make clean ; /usr/bin/time make main14.elf)

V/main14.elf:
	(cd V; make clean ; /usr/bin/time make main14.elf)

bench: $(TARGET)
	/usr/bin/time Rust/main14.elf 1024 768 10 main14_rust.ppm && md5sum main14_rust.ppm
	/usr/bin/time Nelua/main14.elf 1024 768 10 main14_nelua.ppm && md5sum main14_nelua.ppm
	/usr/bin/time Nim/main14.elf 1024 768 10 main14_nim.ppm && md5sum main14_nim.ppm
	/usr/bin/time C/main14.elf 1024 768 10 main14_c.ppm && md5sum main14_c.ppm
	/usr/bin/time CPP/main14.elf 1024 768 10 main14_cpp.ppm && md5sum main14_cpp.ppm
	/usr/bin/time Odin/main14.elf 1024 768 10 main14_odin.ppm && md5sum main14_odin.ppm
	/usr/bin/time V/main14.elf 1024 768 10 main14_v.ppm && md5sum main14_v.ppm
	/usr/bin/time Go/main14.elf 1024 768 10 main14_go.ppm && md5sum main14_go.ppm

clean:
	$(RM) $(TARGET)

clobber: clean
	$(RM) *_v.c

mrproper: clobber
	$(RM) *.ppm
	$(MAKE) -C Rust clean
	$(MAKE) -C Nelua clean
	$(MAKE) -C Nim clean
	$(MAKE) -C C clean
	$(MAKE) -C CPP clean
	$(MAKE) -C Odin clean
	$(MAKE) -C V clean
	$(MAKE) -C Go clean
