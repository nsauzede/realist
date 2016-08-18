package main

import (
	"fmt"
	"vec"
)

var w = 10
var h = 10
var nobj = 4

var s, e vec.V3

func Solvetri(a, b, c float32) (sol int, t1, t2 float32) {
	return
}
func Intersec(s, o, v vec.V3) (t float32) {
	t = v.Dot(v)
	return
}

func Trace(o, v vec.V3) (r, g, b float32) {
	for i := 0; i < nobj; i++ {
		Intersec(s, o, v)
	}
	return
}

func Render() {
	for j := 0; j < h; j++ {
		for i := 0; i < w; i++ {
			var v vec.V3
			r, g, b := Trace(e, v)
			fmt.Printf(" %3.f,%3.f,%3.f", 100*r, 100*g, 100*b)
		}
		fmt.Println()
	}
}

func Draw() {
	for j := 0; j < h; j++ {
		for i := 0; i < w; i++ {
//			fmt.Print(".")
		}
//		fmt.Println()
	}
}

func main() {
	Render()
	Draw()
}
