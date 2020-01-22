#include "random.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>

inline float pdf(const vec3& p) {
    return 1 / (4*M_PI);
}

int main() {
    int N = 1000000;
    float sum;
    for (int i = 0; i < N; i++) {
            vec3 d = random_on_unit_sphere();
            float cosine_squared = d.z()*d.z();
            sum += cosine_squared / pdf(d);
    }
    std::cout << "I =" << sum/N << "\n";
    printf("PI=%.20f\n", M_PI);
}
