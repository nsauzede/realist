# Ray tracing in one weekend
See here : https://raytracing.github.io
This repo contains my experiments following the excellent book above,
implemented in Rust, Nim, C, C++, Odin, V and Go. (sorted by runtime perf)

# rtiow
Benchmark :
-------------
`$ make mrproper bench`

```
/usr/bin/time Rust/main14.elf 1024 768 10 main14_rust.ppm && md5sum main14_rust.ppm
34.92user 0.00system 0:34.94elapsed 99%CPU (0avgtext+0avgdata 4932maxresident)k
/usr/bin/time Nim/main14.elf 1024 768 10 main14_nim.ppm && md5sum main14_nim.ppm
42.10user 0.00system 0:42.13elapsed 99%CPU (0avgtext+0avgdata 4340maxresident)k
/usr/bin/time C/main14.elf 1024 768 10 main14_c.ppm && md5sum main14_c.ppm
50.84user 0.00system 0:51.10elapsed 99%CPU (0avgtext+0avgdata 4524maxresident)k
/usr/bin/time CPP/main14.elf 1024 768 10 main14_cpp.ppm && md5sum main14_cpp.ppm
57.68user 0.00system 0:57.95elapsed 99%CPU (0avgtext+0avgdata 5860maxresident)k
/usr/bin/time Odin/main14.elf 1024 768 10 main14_odin.ppm && md5sum main14_odin.ppm
71.26user 0.01system 1:11.31elapsed 99%CPU (0avgtext+0avgdata 4220maxresident)k
/usr/bin/time V/main14.elf 1024 768 10 main14_v.ppm && md5sum main14_v.ppm
73.52user 0.00system 1:13.56elapsed 99%CPU (0avgtext+0avgdata 4620maxresident)k
/usr/bin/time Go/main14.elf 1024 768 10 main14_go.ppm && md5sum main14_go.ppm
312.65user 1.38system 5:11.39elapsed 100%CPU (0avgtext+0avgdata 10116maxresident)k
```

Conclusion (old) :
-------------------
- V version is slower than C++ (+89%, x1.85)
- This is both suspect and frustrating

NOTE: current V (AST) is buggy; only v0.1.25 is know to work.

```
$ /usr/bin/time V/A14 > A14.ppm
26.43user 0.00system 0:26.46elapsed 99%CPU (0avgtext+0avgdata 3108maxresident)k
0inputs+488outputs (0major+323minor)pagefaults 0swaps
$ /usr/bin/time CPP/main14 > main14.ppm
13.98user 0.00system 0:13.99elapsed 99%CPU (0avgtext+0avgdata 3904maxresident)k
0inputs+528outputs (0major+149minor)pagefaults 0swaps
```

Notes :
2/output an image
-output text PPM image RGB888
-graphics hello world : R:0=>1 LTR, G:0=>1 BTT; G=>Y, B=>R

3/the vec3 class
-vec3 class : color,location,direction,offset,...
-operators : `+,-,[],&[], +=,-=,*=,/=,*=f,/=f`

TODO :
-understand why V version is slower and fix it

# rttnw vs. rttroyl
Tests revealed that rttnw (brute force) produce more noisy result
that rttroyl (stochastic)

rttnw   500 500 1000 => 7min lot of noise
rttroyl 500 500 1000 => 8min less noise

rttnw   500 500 2000 => 15min still more noise
