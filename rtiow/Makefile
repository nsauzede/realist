TARGET:=V/A14_v
TARGET+=V/A14_v.c
TARGET+=CPP/main14

all: $(TARGET)

CPP/main14: CPP/main14.cpp
	(cd CPP; g++ -o main14 main14.cpp -O3 -lm)

V/A14_v: V/A14_v.c
	(cd V; gcc -o A14_v A14_v.c -O3 -lm)

V/A14_v.c: V/A14.v
	(cd V; v -o A14_v.c A14.v)

bench: $(TARGET)
	/usr/bin/time V/A14_v 200 100 100 A14.ppm
	/usr/bin/time CPP/main14 200 100 100 main14.ppm

clean:
	$(RM) $(TARGET)

clobber: clean
	$(RM) *_v.c

mrproper: clobber
	$(RM) *.ppm
	$(MAKE) -C C clean
	$(MAKE) -C V clean
	$(MAKE) -C CPP clean
