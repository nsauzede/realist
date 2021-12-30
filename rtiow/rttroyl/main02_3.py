import math
import random
def random_double(a=0,b=1):
	return (b-a)*random.random()+a

sqrt_N=10000
inside=0
inside_strat=0
for i in range(sqrt_N):
	for j in range(sqrt_N):
		x=random_double(-1,1)
		y=random_double(-1,1)
		if x*x+y*y<1:inside+=1
		x=2*((i+random_double()) / sqrt_N)-1
		y=2*((j+random_double()) / sqrt_N)-1
		if x*x+y*y<1:inside_strat+=1
print(f'Regular    Estimate of Pi = {4*inside/sqrt_N/sqrt_N:.12f}')
print(f'Stratified Estimate of Pi = {4*inside_strat/sqrt_N/sqrt_N:.12f}')
