#include "random.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>

int main0() {
    int N = 1000;
    int inside_circle = 0;
    for (int i = 0; i < N; i++) {
        float x = 2*random_double() - 1;
        float y = 2*random_double() - 1;
        if(x*x + y*y < 1)
            inside_circle++;
    }
    std::cout << "Estimate of Pi = " << 4*float(inside_circle) / N << "\n";
}

int main1() {
    long inside_circle = 0;
    long runs = 0;
    while (true) {
        runs++;
        float x = 2*random_double() - 1;
        float y = 2*random_double() - 1;
        if(x*x + y*y < 1)
            inside_circle++;

        if(runs% 100000 == 0)
            std::cout << "Estimate of Pi = " << 4*float(inside_circle) / runs << "\n";

    }
}

int main() {
    int inside_circle = 0;
    int inside_circle_stratified = 0;
    int sqrt_N = 10000;
    for (int i = 0; i < sqrt_N; i++) {
        for (int j = 0; j < sqrt_N; j++) {
            float x = 2*random_double() - 1;
            float y = 2*random_double() - 1;
            if (x*x + y*y < 1)
                inside_circle++;
            x = 2*((1 + random_double()) / sqrt_N) - 1;
            y = 2*((1 + random_double()) / sqrt_N) - 1;
            if (x*x + y*y < 1)
                inside_circle_stratified++;
        }
    }
    std::cout << "Regular    Estimate of Pi = " <<
          4*float(inside_circle) / (sqrt_N*sqrt_N) << "\n";
    std::cout << "Stratified Estimate of Pi = " <<
          4*float(inside_circle_stratified) / (sqrt_N*sqrt_N) << "\n";
}
