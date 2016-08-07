#!/mingw64/bin/python3
import math
import numpy.matlib as np

w=100
h=100
ww=1.
hh=ww*h/w
tab=[['.' for x in range(w)] for y in range(h)]
ED=3
e=np.array([ED,ED,ED],dtype=float)
f=np.array([-1,-1,-1],dtype=float)
u=np.array([0,0,1],dtype=float)
SR=0.1
sphs=[]
sphs+=[np.array([0,0,0,SR,1,1,1],dtype=float)]
sphs+=[np.array([1,0,0,SR,1,0,0],dtype=float)]
sphs+=[np.array([0,1,0,SR,0,1,0],dtype=float)]
sphs+=[np.array([0,0,1,SR,0,0,1],dtype=float)]
u/=np.linalg.norm(u)
r=np.cross(f,u)
u=np.cross(r,f)
u/=np.linalg.norm(u)
r/=np.linalg.norm(r)

def SolveTri(a,b,c):
	d=b*b-4*a*c
	t1=0.
	t2=0.
	sol=0
	if d>0:
		sd=math.sqrt(d)
		t1=(-b-sd)/2/a
		t2=(-b+sd)/2/a
		sol=2
	elif d==0:
		t1=-b/2/a
		sol=1
	return sol,t1,t2

HUGE_VAL=1e100
def Intersec(s,o,v):
	c=np.array(s[0:3])
	rad=s[3]
	vt=o-c
	a=np.dot(v,v)
	b=2*np.dot(v,vt)
	c=np.dot(vt,vt)-rad*rad
	sol,t1,t2=SolveTri(a,b,c)
	if sol==2:
		if t1<t2:
			t=t1
		else:
			t=t2
	elif sol==1:
		t=t1
	else:
		t=HUGE_VAL
	return t

def Trace(o,v):
	tmin=HUGE_VAL
	omin=0
	for s in sphs:
		t=Intersec(s,o,v)
		if t>0 and t<tmin:
			tmin=t
			omin=s
	if tmin<HUGE_VAL:
		if omin[4]==1:
			if omin[5]==1:
				if omin[6]==1:
					col='W'
				else:
					col='M'
			elif omin[6]==1:
				col='V'
			else:
				col='R'
		elif omin[5]==1:
			if omin[6]==1:
				col='Y'
			else:
				col='G'
		elif omin[6]==1:
			col='B'
		else:
			col='K'
	else:
		col=0
	return col

for j in range(h):
	vu=u*(j-h/2)/h*hh
	for i in range(w):
		vr=r*(i-w/2)/w*ww
		v=f+vu+vr
		v/=np.linalg.norm(v)
		col=Trace(e,v)
		tab[h-j-1][i]=col

for row in tab:
	for e in row:
		print(e,end="")
	print("")
