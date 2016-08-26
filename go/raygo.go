package main

import (
	"fmt"
	"math"
	"os"
	"vec"
	"container/list"
//	"reflect"
)

var w = 10
var h = 10
var ratiox = 1.
var ratioy = 1.
var ww = 1. * ratiox
var hh = ww * float64(h) / float64(w) * ratioy
var nobj = 4
var e = vec.Vector { 0.4, 0, 0.4 }
var f = vec.Vector { -1, 0, -1 }
var u = vec.Vector { -0.707107, 0, 0.707107 }
var sphs *list.List

func SolveTri(a, b, c float64) (sol int, t1, t2 float64) {
	d := b * b - 4. * a * c
//	fmt.Println("d=",d)
	if d > 0 {
		sd := math.Sqrt( d)
		t1 = (-b - sd) / 2 / a;
		t2 = (-b + sd) / 2 / a;
		sol = 2
	} else if d == 0 {
		t1 = -b / 2 /a
		sol = 1
	}
	return
}
func Intersec(s []float64, o, v vec.Vector) (t float64) {
	center := vec.Vector{s[0], s[1], s[2]}
	rad := s[3]
	vt := o.Sub( center)
	a := v.Dot( v)
	b := 2 * v.Dot( vt)
	c := vt.Dot( vt) - rad * rad
	sol, t1, t2 := SolveTri( a, b, c)
//	if sol > 2 {
//		fmt.Println("center=",center)
//		fmt.Println("rad=",rad)
//		fmt.Println("vt=",vt)
//		fmt.Printf("a=%f b=%f c=%f\n",a,b,c)
//		fmt.Printf("sol=%d t1=%f t2=%f\n",sol,t1,t2)
//		os.Exit( 1)
//	}
	if sol == 2 {
		if t1 < t2 {
			t = t1
		} else {
			t = t2
		}
	} else if sol == 1 {
		t = t1
	} else {
		t = math.MaxFloat64
	}
	return
}

func Trace(o, v vec.Vector) (rr, gg, bb float64) {
	tmin := math.MaxFloat64
	var omin []float64
	for e := sphs.Front(); e != nil; e = e.Next() {
		s := e.Value.([]float64)
		t := Intersec(s, o, v)
		if t > 0 && t < tmin {
			tmin = t
			omin = s
		}
	}
	if tmin < math.MaxFloat64 {
		rr, gg, bb = omin[4], omin[5], omin[6]
	}
	return
}

func Render() {
	sphs = list.New()
	sphs.PushBack([]float64{ 0, -0.1, 0, 0.05, 0.8, 0.8, 0.8})
	sphs.PushBack([]float64{ 0, 0, 0, 0.05,   0.8, 0.8, 0.8})
	sphs.PushBack([]float64{ 0, 0.1, 0,   0.05,   0.8, 0.8, 0.8})
	sphs.PushBack([]float64{ 0.1, -0.05, 0,   0.05,   0.8, 0, 0})
	sphs.PushBack([]float64{ 0.1, 0.05, 0,    0.05,   0, 0, 0.8})
	sphs.PushBack([]float64{ 0.2, 0, 0,   0.05,   0, 0.8, 0})
	sphs.PushBack([]float64{ 0.05, -0.05, 0.1,    0.05,   0.8, 0.8, 0.8})
	sphs.PushBack([]float64{ 0.05, 0.05, 0.1, 0.05,   0.8, 0.8, 0.8})
	sphs.PushBack([]float64{ 0, -0.5, 0.5,    0.02,   1, 1, 0})
	u.Normalize()
	var r = f.Cross( u)
	u = r.Cross( f)
	u.Normalize()
	r.Normalize()

	fmt.Println("P3")
//	fmt.Println("# raygo")
//	fmt.Println("#e=", e)
//	fmt.Println("#f=", f)
//	fmt.Println("#u=", u)
	fmt.Printf("%d %d\n", w, h)
	fmt.Println(100)
	for j := 0; j < h; j++ {
		vu := u.Mult((float64(h) - float64(j) - 1 - float64(h) / 2) / float64(h) * hh)
		for i := 0; i < w; i++ {
			vr := r.Mult((float64(i) - float64(w) / 2) / float64(w) * ww)
			v := f.Add( vu.Add( vr))
			v.Normalize()
			rr, gg, bb := Trace(e, v)
			fmt.Printf("%2.f %2.f %2.f   ", 100*rr, 100*gg, 100*bb)
		}
		fmt.Println()
	}
}

func main() {
	argv := os.Args
	arg, argc := 1, len(argv)
	if arg < argc {
		fmt.Sscanf(argv[arg], "%d", &w)
		arg++
		if arg < argc {
			fmt.Sscanf(argv[arg], "%d", &h)
			arg++
		}
	}
	Render()
}
