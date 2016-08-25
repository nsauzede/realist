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

func Solvetri(a, b, c float64) (sol int, t1, t2 float64) {
	return
}
func Intersec(s *list.Element, o, v vec.Vector) (t float64) {
	t = v.Dot(v)
	return
}

func Trace(o, v vec.Vector) (r, g, b float64) {
//	fmt.Println( "v=", v)
//	os.Exit( 1)
	tmin := math.MaxFloat64
	var omin vec.Vector
//	for i := 0; i < nobj; i++ {
	for s := sphs.Front(); s != nil; s = s.Next() {
		t := Intersec(s, o, v)
		if t > 0 && t < tmin {
			tmin = t
			omin = s.Value.(vec.Vector)
//			omin = 2.
//			fmt.Println( reflect.TypeOf( s.Value))
//			fmt.Println( s.Value)
		}
	}
	if tmin < math.MaxFloat64 {
//		fmt.Println( omin)
		r, g, b = omin.X, omin.Y, omin.Z
	}
	return
}

func Render() {
	sphs = list.New()
	sphs.PushBack(vec.Vector{0.1, 0.2, 0.3})
	u.Normalize()
	var r = f.Cross( u)
	u = r.Cross( f)
	u.Normalize()
	r.Normalize()

	fmt.Println("P3")
	fmt.Println("# raygo")
	fmt.Println("#e=", e)
	fmt.Println("#f=", f)
	fmt.Println("#u=", u)
	fmt.Printf("%d %d\n", w, h)
	fmt.Println(100)
	for j := 0; j < h; j++ {
		vu := u.Mult((float64(h) - float64(j) - 1 - float64(h) / 2) / float64(h) * hh)
//		fmt.Println( "vu=", vu)
		for i := 0; i < w; i++ {
			vr := r.Mult((float64(i) - float64(w) / 2) / float64(w) * ww)
//			fmt.Println( "vr=", vr)
			v := f.Add( vu.Add( vr))
			v.Normalize()
			rr, gg, bb := Trace(e, v)
			//fmt.Printf(" %3.f,%3.f,%3.f", 100*rr, 100*gg, 100*bb)
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
