DIRS:=
DIRS+=Rust
DIRS+=Nelua
DIRS+=Nim
DIRS+=C
DIRS+=CPP
DIRS+=Odin
DIRS+=V
DIRS+=Go
TARGETS:=$(patsubst %,%/main14.elf,$(DIRS))

all: $(TARGETS)

%/main14.elf:
	(cd $(@D); make clean ; /usr/bin/time make main14.elf) 

bench: all
	for d in $(DIRS); do make -C $$d bench; done

clean:
	$(RM) $(TARGETS)

clobber: clean
	$(RM) *_v.c

mrproper: clobber
	$(RM) *.ppm
	for d in $(DIRS); do make -C $$d clean; done
