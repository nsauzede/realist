import math
import random
def random_double():return random.random()*2-1

sqrt_N=10000
inside=0
inside_strat=0
for i in range(sqrt_N):
    x=random_double()
    y=random_double()
    if x*x+y*y<1:inside+=1
print(f'Estimate of Pi = {4*inside/N:.10f}')
