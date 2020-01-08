Ray tracing in one weekend
==========================
See here : https://raytracing.github.io
This repo contains my experiments following the excellent book above,
in C++ and V

2/output an image
-----------------
-output text PPM image RGB888
-graphics hello world : R:0=>1 LTR, G:0=>1 BTT; G=>Y, B=>R
3/the vec3 class
----------------
-vec3 class : color,location,direction,offset,...
-operators : `+,-,[],&[], +=,-=,*=,/=,*=f,/=f`

Conclusion
- rtiow
V version is slower than C++ (+89%, x1.85)
This is both suspect and frustrating

$ /usr/bin/time ./A14 > A14.ppm
26.43user 0.00system 0:26.46elapsed 99%CPU (0avgtext+0avgdata 3108maxresident)k
0inputs+488outputs (0major+323minor)pagefaults 0swaps
$ /usr/bin/time ./main14 > main14.ppm
13.98user 0.00system 0:13.99elapsed 99%CPU (0avgtext+0avgdata 3904maxresident)k
0inputs+528outputs (0major+149minor)pagefaults 0swaps


TODO :
-understand why V version is slower and fix it
-port rtiow to pure C, then see its perf and if a related V version can be derived

- rttnw vs. rttroyl
Tests revealed that rttnw (brute force) produce more noisy result
that rttroyl (stochastic)

rttnw   500 500 1000 => 7min lot of noise
rttroyl 500 500 1000 => 8min less noise

rttnw   500 500 2000 => 15min still more noise
