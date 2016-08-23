package main

import (
	"fmt"
	"os"
	"vec"
)

var w = 10
var h = 10
var ratiox = 1.
var ratioy = 1.
var ww = 1. * ratiox
var hh = ww * float64(h) / float64(w) * ratioy
var nobj = 4
var e vec.Vector
var f vec.Vector
var u vec.Vector
var s vec.Vector
var r vec.Vector

func Solvetri(a, b, c float64) (sol int, t1, t2 float64) {
	return
}
func Intersec(s, o, v vec.Vector) (t float64) {
	t = v.Dot(v)
	return
}

func Trace(o, v vec.Vector) (r, g, b float64) {
	for i := 0; i < nobj; i++ {
		Intersec(s, o, v)
	}
	return
}

func Render() {
	for j := 0; j < h; j++ {
		//		vu := u * (h - j - 1 - h / 2) / h * hh
		vu := u.Mult((float64(h) - float64(j) - 1 - float64(h)/2) / float64(h) * hh)
		for i := 0; i < w; i++ {
			vr := r.Mult((float64(i)-float64(w)/2)/float64(w)*ww)
			v := f.Add(vu.Add(vr))
			r, g, b := Trace(e, v)
			fmt.Printf(" %3.f,%3.f,%3.f", 100*r, 100*g, 100*b)
		}
		fmt.Println()
	}
}

func main() {
	argv := os.Args
	arg, argc := 1, len(argv)
	fmt.Printf("argc=%d\n", argc)
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
