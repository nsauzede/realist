# Ray tracing in one weekend
See here : https://raytracing.github.io
This repo contains my experiments following the excellent book above,
implemented in Rust, Nelua, Nim, C, C++, Odin, V and Go. (sorted by runtime perf)

Note : the C current perf is poor, probably because it's mostly a dirty port of the C++ design,
with vtables to simulates OOP constructs. A rework in underway to try and get decent perfs.

# rtiow
Benchmark :
-------------
`$ make mrproper bench`

```
/usr/bin/time Rust/main14.elf 1024 768 10 main14_rust.ppm && md5sum main14_rust.ppm
34.88user 0.01system 0:34.91elapsed 99%CPU (0avgtext+0avgdata 4708maxresident)k
8b59ffdb7405d4d7b1ad2c64f7d005ec  main14_rust.ppm
/usr/bin/time Nelua/main14.elf 1024 768 10 main14_nelua.ppm && md5sum main14_nelua.ppm
35.70user 0.01system 0:35.72elapsed 99%CPU (0avgtext+0avgdata 4528maxresident)k
8b59ffdb7405d4d7b1ad2c64f7d005ec  main14_nelua.ppm
/usr/bin/time Nim/main14.elf 1024 768 10 main14_nim.ppm && md5sum main14_nim.ppm
41.91user 0.00system 0:42.14elapsed 99%CPU (0avgtext+0avgdata 4424maxresident)k
8b59ffdb7405d4d7b1ad2c64f7d005ec  main14_nim.ppm
/usr/bin/time C/main14.elf 1024 768 10 main14_c.ppm && md5sum main14_c.ppm
50.70user 0.00system 0:50.72elapsed 99%CPU (0avgtext+0avgdata 4488maxresident)k
8b59ffdb7405d4d7b1ad2c64f7d005ec  main14_c.ppm
/usr/bin/time CPP/main14.elf 1024 768 10 main14_cpp.ppm && md5sum main14_cpp.ppm
57.31user 0.00system 0:57.33elapsed 99%CPU (0avgtext+0avgdata 5684maxresident)k
8b59ffdb7405d4d7b1ad2c64f7d005ec  main14_cpp.ppm
/usr/bin/time Odin/main14.elf 1024 768 10 main14_odin.ppm && md5sum main14_odin.ppm
70.59user 0.01system 1:10.63elapsed 99%CPU (0avgtext+0avgdata 4340maxresident)k
8b59ffdb7405d4d7b1ad2c64f7d005ec  main14_odin.ppm
/usr/bin/time V/main14.elf 1024 768 10 main14_v.ppm && md5sum main14_v.ppm
73.38user 0.01system 1:13.43elapsed 99%CPU (0avgtext+0avgdata 4516maxresident)k
38f352c28f5e4b3c2dc06bb100ef9934  main14_v.ppm
/usr/bin/time Go/main14.elf 1024 768 10 main14_go.ppm && md5sum main14_go.ppm
337.17user 3.74system 5:40.56elapsed 100%CPU (0avgtext+0avgdata 10648maxresident)k
73db20b69134a7916e2d94f2291f3a00  main14_go.ppm
```
