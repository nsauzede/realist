# Ray tracing in one weekend
See here : https://raytracing.github.io
This repo contains my experiments following the excellent book above,
implemented in Rust, Nelua, Nim, C, V, C++, Odin ~~and Go~~. (sorted by runtime perf)

Note : the current C perf is poor, probably because it's mostly a dirty port of the C++ design,
with vtables to simulates OOP constructs. A rework in underway to try and get decent perfs.

Note : now that I added superfine support and its 10 runs, I removed Go from the benchmark as it
takes 5x longer than the other slowest lang, until I (or someone) can fix its impl to get decent speed.

# rtiow
Benchmark :
-------------
`$ make mrproper bench`

```
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Rust'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
34.56user 0.00system 0:34.57elapsed 99%CPU (0avgtext+0avgdata 4624maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Nelua'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
35.82user 0.00system 0:35.83elapsed 99%CPU (0avgtext+0avgdata 4292maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Nim'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
41.66user 0.00system 0:41.67elapsed 99%CPU (0avgtext+0avgdata 4164maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/C'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
50.40user 0.00system 0:50.42elapsed 99%CPU (0avgtext+0avgdata 4340maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/V'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
55.69user 0.09system 0:55.80elapsed 99%CPU (0avgtext+0avgdata 250048maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/CPP'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
56.94user 0.00system 0:56.95elapsed 99%CPU (0avgtext+0avgdata 5592maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Odin'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
69.75user 0.00system 1:09.77elapsed 99%CPU (0avgtext+0avgdata 4204maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Go'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
329.23user 2.03system 5:28.69elapsed 100%CPU (0avgtext+0avgdata 10104maxresident)k
```

It can help to first do:
```
sudo cpupower frequency-set -g performance
```
