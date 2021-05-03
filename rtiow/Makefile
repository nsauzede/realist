DIRS:=
DIRS+=Rust
DIRS+=Nelua
DIRS+=Nim
DIRS+=C
DIRS+=V
DIRS+=CPP
#DIRS+=Odin
#DIRS+=V_old
#DIRS+=Go
TARGETS:=$(patsubst %,%/main14.elf,$(DIRS))

all: $(TARGETS)

%/main14.elf:
	(cd $(@D); make clean ; time make main14.elf) 

bench: all
	for d in $(DIRS); do make -C $$d bench; done

#./main14.elf 1024 768 10 main14.ppm
#HYP:=$(patsubst %,'cd %;./main14.elf 1024 768 10 main14.ppm',$(DIRS))
#HYP:=$(patsubst %,'cd %;./main14.elf 800 600 8 main14.ppm',$(DIRS))
HYP:=$(patsubst %,'cd %;./main14.elf 640 480 10 main14.ppm',$(DIRS))
#HYP:='echo 1' 'echo 2' 'echo 3'
bench2: all
	echo "HYP=$(HYP)"
#	for c in "$(HYP)"; do echo "c=$$c"; done
#	hyperfine --warmup 2 $(HYP)
	/usr/bin/time hyperfine $(HYP)

clean:
	$(RM) $(TARGETS)

clobber: clean
	$(RM) *_v.c

mrproper: clobber
	$(RM) *.ppm
	for d in $(DIRS); do make -C $$d clean; done
	make -C Forth clean
	make -C C2 clean
	make -C V_old clean
	make -C Go clean
