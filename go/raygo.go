package main

import (
	"fmt"
	"vec"
)

var w = 3
var h = 3
var nobj = 4

var s, o vec.Vector

func Solvetri(a, b, c float32) (sol int, t1, t2 float32) {
	return
}
func Intersec(s, o, v vec.Vector) (t float32) {
	t = v.Dot(v)
	return
}

func Trace() (r, g, b float32) {
	for i := 0; i < nobj; i++ {
		var v vec.Vector
		Intersec(s, o, v)
	}
	return
}

func Render() {
	for j := 0; j < h; j++ {
		for i := 0; i < w; i++ {
			Trace()
		}
	}
}

func Draw() {
	for j := 0; j < h; j++ {
		for i := 0; i < w; i++ {
			fmt.Print(".")
		}
		fmt.Println()
	}
}

func main() {
	Render()
	Draw()
}
