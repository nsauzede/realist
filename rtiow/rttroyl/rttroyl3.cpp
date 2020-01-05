#include "random.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>

int main0() {
    int N = 1000000;
    float sum;
    for (int i = 0; i < N; i++) {
        float x = 2*random_double();
        sum += x*x;
    }
    std::cout << "I =" << 2*sum/N << "\n";
}

inline float pdf0(float x) {
    return 0.5*x;
}

int main1() {
    int N = 1000000;
    float sum;
    for (int i = 0; i < N; i++) {
        float x = sqrt(4*random_double());
        sum += x*x / pdf0(x);
    }
    std::cout << "I =" << sum/N << "\n";
}

int main2() {
    int N = 1000000;
    float sum;
    for (int i = 0; i < N; i++) {
        float x = 2*random_double();
        sum += x*x / pdf0(x);
    }
    std::cout << "I =" << sum/N << "\n";
}

inline float pdf(float x) {
    return 3*x*x/8;
}

int main() {
    int N = 1;
    float sum;
    for (int i = 0; i < N; i++) {
        float x = pow(8*random_double(), 1./3.);
        sum += x*x / pdf(x);
    }
    std::cout << "I =" << sum/N << "\n";
}
