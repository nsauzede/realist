import math
import random
def random_double():return random.random()*2-1

N=1000
inside=0
for i in range(N):
    x=random_double()
    y=random_double()
    if x*x+y*y<1:inside+=1
print(f'Estimate of Pi = {4*inside/N:.10f}')
