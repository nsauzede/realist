TARGET:=A14_v
TARGET+=A14_v.c
TARGET+=main14

all: $(TARGET)

main14: main14.cpp
	g++ -o main14 main14.cpp -O3 -lm

A14_v: A14_v.c
	gcc -o A14_v A14_v.c -O3 -lm

A14_v.c: A14.v
	v -o A14_v.c A14.v

bench: $(TARGET)
	/usr/bin/time ./A14_v 200 100 100 A14.ppm
	/usr/bin/time ./main14 200 100 100 main14.ppm

clean:
	$(RM) $(TARGET)
