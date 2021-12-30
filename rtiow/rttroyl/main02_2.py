import math
import random
def random_double():return random.random()*2-1

runs=0
inside=0
while True:
    runs+=1
    x=random_double()
    y=random_double()
    if x*x+y*y<1:inside+=1
    print(f'run {runs}: Estimate of Pi = {4*inside/runs:.10f}')
