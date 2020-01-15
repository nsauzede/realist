# realist
Simple, naive, C++ ray-tracer

# rayXX
Even simpler ray-tracer benchmark to compare between C, C++, Vlang and golang.

To run the benchmark :
```
$ make clobber bench
gcc -Wall -Werror -Wextra -g -pipe -std=c99 -O3  -pipe  rayc.c  -lm -o rayc
g++ -Wall -Werror -Wextra -g -pipe -std=c++11 -O3 -I.  -pipe  raycpp.cpp  -lm -o raycpp
cd ray_v ; ../v/v  -o ../rayv_v.c . ; cd ..
gcc -Wall -Werror -Wextra -g -pipe -std=c99 -O3  -pipe rayv_v.c -lm -o rayv -w
GOPATH=`pwd`/ray_go go build -o raygo ray_go/raygo.go
/usr/bin/time ./rayc 3000 3000 > rayc.ppm && md5sum rayc.ppm
4.90user 0.10system 0:05.02elapsed 99%CPU (0avgtext+0avgdata 1916maxresident)k
0inputs+193368outputs (0major+82minor)pagefaults 0swaps
bbf21395da0daa2b218009f95f51158c  rayc.ppm
/usr/bin/time ./raycpp 3000 3000 > raycpp.ppm && md5sum raycpp.ppm
6.91user 0.14system 0:07.08elapsed 99%CPU (0avgtext+0avgdata 3596maxresident)k
0inputs+193368outputs (0major+138minor)pagefaults 0swaps
bbf21395da0daa2b218009f95f51158c  raycpp.ppm
/usr/bin/time ./rayv 3000 3000 > rayv.ppm && md5sum rayv.ppm
4.96user 0.17system 0:05.15elapsed 99%CPU (0avgtext+0avgdata 380892maxresident)k
0inputs+193368outputs (0major+73907minor)pagefaults 0swaps
bbf21395da0daa2b218009f95f51158c  rayv.ppm
/usr/bin/time ./raygo 3000 3000 > raygo.ppm && md5sum raygo.ppm
8.52user 0.17system 0:08.95elapsed 97%CPU (0avgtext+0avgdata 10424maxresident)k
0inputs+193368outputs (0major+366minor)pagefaults 0swaps
bbf21395da0daa2b218009f95f51158c  raygo.ppm
```

# Acknowledgements
Many thanks to spytheman (delian66@gmail.com) for his significant improvements
done to the V port (both perfs and accuracy).
Thanks to Amaury for initial rust port (WIP)
