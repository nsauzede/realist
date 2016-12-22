#!/usr/bin/python3
import math
import numpy.matlib as np
import sys

fin=open(sys.argv[1],"r")
try:
	json=eval(fin.read())
finally:
	fin.close()
screen=json['screen']
camera=json['camera']
objects=json['objects']

fnameout=""
w=screen['w']
h=screen['h']
ratiox=screen['ratiox']
ratioy=screen['ratioy']
if len(sys.argv) > 2:
	w=int(sys.argv[2])
if len(sys.argv) > 3:
	h=int(sys.argv[3])
if len(sys.argv) > 4:
	fnameout=sys.argv[4]
if len(sys.argv) > 5:
	ratiox=float(sys.argv[5])
if len(sys.argv) > 6:
	ratioy=float(sys.argv[6])
ww=1.*ratiox
hh=ww*h/w*ratioy
e=np.array(camera['loc'],dtype=float)
f=np.array(camera['front'],dtype=float)
u=np.array(camera['up'],dtype=float)
sphs=[]
for sph in objects:
	sphs += [np.array(sph['data'],dtype=float)]
u/=np.linalg.norm(u)
r=np.cross(f,u)
u=np.cross(r,f)
u/=np.linalg.norm(u)
r/=np.linalg.norm(r)

def SolveTri(a,b,c):
	d=b*b-4*a*c
	t1,t2 = 0., 0.
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
	cent=np.array(s[0:3])
	rad=s[3]
	vt=o-cent
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
#	i=0
	for s in sphs:
		t=Intersec(s,o,v)
		if t>0 and t<tmin:
			tmin=t
			omin=s
#			imin=i
#		i+=1
	if tmin<HUGE_VAL:
#		print("[HIT %u]" % imin,end="")
		rr, gg, bb = omin[4], omin[5], omin[6]
	else:
		rr,gg,bb = 0, 0, 0
	return rr,gg,bb

def Render():
	fout=getattr(sys.stdout,'buffer',sys.stdout)	# get binary buffer of stdout
	if fnameout != "":
		fout = open(fnameout,"wb")
	fout.write(b"P3\n")
	fout.write(str.encode("%s %s\n" % (w, h)))
	fout.write(b"100\n")
	for j in range(h):
		vu=u*(h-j-1-h/2)/h*hh
#		print("raypy vu=%s" % vu)
#		input()
		for i in range(w):
			vr=r*(i-w/2)/w*ww
			v=f+vu+vr
			v/=np.linalg.norm(v)
			rr,gg,bb=Trace(e,v)
			fout.write(str.encode("%2.f %2.f %2.f   "%(100*rr,100*gg,100*bb)))
		fout.write(b"\n")
Render()
