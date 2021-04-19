# Ray tracing in one weekend
See here : https://raytracing.github.io
This repo contains my experiments following the excellent book above,
implemented in Rust, Nelua, Nim, C, V, C++, Odin and Go. (sorted by runtime perf)

Note : the current C perf is poor, probably because it's mostly a dirty port of the C++ design,
with vtables to simulates OOP constructs. A rework in underway to try and get decent perfs.

# rtiow
Benchmark :
-------------
`$ make mrproper bench`

```
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Rust'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
34.55user 0.00system 0:34.56elapsed 99%CPU (0avgtext+0avgdata 4772maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Nelua'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
35.67user 0.00system 0:35.68elapsed 99%CPU (0avgtext+0avgdata 4472maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Nim'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
41.56user 0.01system 0:41.59elapsed 99%CPU (0avgtext+0avgdata 4356maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/C'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
53.47user 0.00system 0:53.48elapsed 99%CPU (0avgtext+0avgdata 4248maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/V2'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
55.74user 0.08system 0:55.84elapsed 99%CPU (0avgtext+0avgdata 249956maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/CPP'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
57.04user 0.00system 0:57.05elapsed 99%CPU (0avgtext+0avgdata 5748maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Odin'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
69.80user 0.00system 1:09.82elapsed 99%CPU (0avgtext+0avgdata 4204maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/V'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
69.32user 0.05system 1:09.40elapsed 99%CPU (0avgtext+0avgdata 4264maxresident)k
make[1]: Entering directory '/home/nico/perso/git/realist/rtiow/Go'
time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm
308.74user 1.61system 5:07.80elapsed 100%CPU (0avgtext+0avgdata 10084maxresident)k
```
