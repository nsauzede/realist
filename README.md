# realist
Simple, naive, C++ ray-tracer

# ray
Even simpler ray-tracer benchmark to compare between C, C++, golang, rust and V languages.

To run the benchmark :
```
$ make bench
/usr/bin/time ./rayc 1000 1000 > rayc.ppm && md5sum rayc.ppm
0.49user 0.00system 0:00.50elapsed 99%CPU (0avgtext+0avgdata 1956maxresident)k
0inputs+21488outputs (0major+81minor)pagefaults 0swaps
ba0387de9931eba68dabc97d4bc4d88f  rayc.ppm
/usr/bin/time ./rayv 1000 1000 > rayv.ppm && md5sum rayv.ppm
0.50user 0.02system 0:00.52elapsed 99%CPU (0avgtext+0avgdata 45532maxresident)k
0inputs+21488outputs (0major+9966minor)pagefaults 0swaps
ba0387de9931eba68dabc97d4bc4d88f  rayv.ppm
/usr/bin/time ./raygo 1000 1000 > raygo.ppm && md5sum raygo.ppm
0.74user 0.00system 0:00.75elapsed 99%CPU (0avgtext+0avgdata 10080maxresident)k
0inputs+21488outputs (0major+191minor)pagefaults 0swaps
ba0387de9931eba68dabc97d4bc4d88f  raygo.ppm
/usr/bin/time ./raypy.py 1000 1000 > raypy.ppm && md5sum raypy.ppm
77.54user 0.04system 1:18.22elapsed 99%CPU (0avgtext+0avgdata 30440maxresident)k
0inputs+21504outputs (0major+5068minor)pagefaults 0swaps
ba0387de9931eba68dabc97d4bc4d88f  raypy.ppm
```

# Acknowledgements
Many thanks to spytheman (delian66@gmail.com) for his significant improvements
done to the V port (both perfs and accuracy).
