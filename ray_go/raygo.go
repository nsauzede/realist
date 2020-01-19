/*
 * Copyright(c) 2016-2019 Nicolas Sauzede (nsauzede@laposte.net)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
package main

import ("fmt"; "math"; "os"; "vec"; "container/list"; "bufio"; /*"reflect"*/)

var sphs *list.List
//var buf *bufio.Writer

func SolveTri(a, b, c float64) (sol int, t1, t2 float64) {
	d := b * b - 4. * a * c
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
//	var imin uint
//	var i uint = 0
	for e := sphs.Front(); e != nil; e = e.Next() {
		s := e.Value.([]float64)
		t := Intersec(s, o, v)
		if t > 0 && t < tmin {
			tmin = t
			omin = s
//			imin = i
		}
//		i++
	}
	if tmin < math.MaxFloat64 {
//		fmt.Fprintf( buf, "[HIT %d]", imin);
		rr, gg, bb = omin[4], omin[5], omin[6]
	}
	return
}

func Render( w, h int, fnameout string) {
	var ratiox = 1.
	var ratioy = 1.
	var ww = 1. * ratiox
	var hh = ww * float64(h) / float64(w) * ratioy
	var e = vec.Vector { 0.4, 0, 0.4 }
	var f = vec.Vector { -1, 0, -1 }
	var u = vec.Vector { -0.707107, 0, 0.707107 }
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

	var err error
	fout := os.Stdout
	if fnameout != "" {
		fout,err = os.Create( fnameout)
		if err != nil {
			panic(err)
		}
		defer fout.Close()
	}
	buf := bufio.NewWriter(fout)
//	buf = bufio.NewWriter(fout)
//	fmt.Println( reflect.TypeOf(buf));
	var bytes []byte
	var nbytes = 0
	if fnameout != "" {
		fmt.Fprintf(buf,"P6\n")
		nbytes = 3 * w * h
		bytes = make([]byte, nbytes)
	} else {
		fmt.Fprintf(buf,"P3\n")
	}
	fmt.Fprintf(buf,"%d %d\n", w, h)
	max := 255
	fmt.Fprintf(buf,"%d\n",max)
	for j := 0; j < h; j++ {
		vu := u.Mult((float64(h) - float64(j) - 1 - float64(h) / 2) / float64(h) * hh)

//		fmt.Fprintf( buf, "raygo vu=%v\n", vu);
//		buf.Flush()
//		var b []byte = make([]byte, 1)
//		os.Stdin.Read( b)

		for i := 0; i < w; i++ {
			vr := r.Mult((float64(i) - float64(w) / 2) / float64(w) * ww)
			v := f.Add( vu.Add( vr))
			v.Normalize()
			rr, gg, bb := Trace(e, v)
			if fnameout != "" {
				bytes[(j * w + i) * 3 + 0] = byte(float64(max)*rr)
				bytes[(j * w + i) * 3 + 1] = byte(float64(max)*gg)
				bytes[(j * w + i) * 3 + 2] = byte(float64(max)*bb)
			} else {
				fmt.Fprintf(buf,"%2.f %2.f %2.f   ", float64(max)*rr, float64(max)*gg, float64(max)*bb)
			}
		}
		if fnameout == "" {
			fmt.Fprintf(buf,"\n")
		}
	}
	if fnameout != "" {
		buf.Write(bytes)
	}
	buf.Flush()	// ???
}

func main() {
	var w, h = 10, 10
	var fnameout = ""
	argv := os.Args
	arg, argc := 1, len(argv)
	if arg < argc {
		fmt.Sscanf(argv[arg], "%d", &w)
		arg++
		if arg < argc {
			fmt.Sscanf(argv[arg], "%d", &h)
			arg++
			if arg < argc {
				fnameout = argv[arg]
				arg++
			}
		}
	}
	Render( w, h, fnameout)
}
