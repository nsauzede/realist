module main

import (
        vec
        os
        math
)

fn solve_tri(a, b, c f64, t mut []f64) int {
        mut sol := 0
        d := b * b - 4. * a * c
        if d > 0 {
                sd := math.sqrt(d)
                t[0] = (-b - sd) / 2 / a
                t[1] = (-b + sd) / 2 / a
                sol = 2
        } else if d == 0 {
                t[0] = -b / 2 / a
                sol = 1
        }
        return sol
}

const (
        math_MaxFloat64 = 9999999999999999999.
)

fn intersec(s []f64, o, v vec.Vector) f64 {
        mut t := f64(0.)
        center := vec.Vector{s[0], s[1], s[2]}
        rad := s[3]
        vt := o.sub(center)
        a := v.dot(v)
        b := 2. * v.dot(vt)
        c := vt.dot(vt) - rad * rad
        mut t12 := [f64(0.); 2]
        sol := solve_tri(a, b, c, mut t12)
        if sol == 2 {
                if t12[0] < t12[1] {
                        t = t12[0]
                } else {
                        t = t12[1]
                }
        } else if sol == 1 {
                t = t12[0]
        } else {
//                t = math.MaxFloat64
                t = math_MaxFloat64
        }
        return t
}

fn trace(sphs [][]f64, o, v vec.Vector, rgb mut []f64) {
//        tmin := math.MaxFloat64
        mut tmin := math_MaxFloat64
        mut omin := [f64(0.); 7]        // FIXME : needs to be big enough for any obj
        for s in sphs {
                t := intersec(s, o, v)
                if t > 0 && t < tmin {
                        tmin = t
                        omin = s
                }
        }
        if tmin < math_MaxFloat64 {
                rgb[0] = omin[4]
                rgb[1] = omin[5]
                rgb[2] = omin[6]
        }
}

fn render(w, h int, fnameout string) {
        ratiox := 1.
        ratioy := 1.
        ww := 1. * ratiox
        hh := ww * f64(h) / f64(w) * ratioy
        e := vec.Vector{0.4, 0, 0.4}
        mut f := vec.Vector{-1, 0, -1}
        mut u := vec.Vector{-0.707107, 0, 0.707107}
        mut sphs := [[]f64; 0]
        sphs << [f64(0), -0.1, 0, 0.05, 0.8, 0.8, 0.8]
        sphs << [f64(0), 0, 0, 0.05,   0.8, 0.8, 0.8]
        sphs << [f64(0), 0.1, 0,   0.05,   0.8, 0.8, 0.8]
        sphs << [f64(0.1), -0.05, 0,   0.05,   0.8, 0, 0]
        sphs << [f64(0.1), 0.05, 0,    0.05,   0, 0, 0.8]
        sphs << [f64(0.2), 0, 0,   0.05,   0, 0.8, 0]
        sphs << [f64(0.05), -0.05, 0.1,    0.05,   0.8, 0.8, 0.8]
        sphs << [f64(0.05), 0.05, 0.1, 0.05,   0.8, 0.8, 0.8]
        sphs << [f64(0), -0.5, 0.5,    0.02,   1, 1, 0]

        u.normalize()
        mut r := f.cross(u)
        u = r.cross(f)
        u.normalize()
        r.normalize()

        println('P3')
        println('$w $h')
        println(100)
        for j := 0; j < h; j++ {
                vu := u.mult((f64(h) - f64(j) - 1 - f64(h) / 2) / f64(h) * hh)

                for i := 0; i < w; i++ {
                        vr := r.mult((f64(i) - f64(w) / 2) / f64(w) * ww)
                        mut v := f.add(vu.add( vr))
                        v.normalize()
                        mut rgb := [f64(0.), f64(0.), f64(0.)]
                        trace(sphs, e, v, mut rgb)
                        rr := int(f64(100.) * rgb[0])
                        gg := int(f64(100.) * rgb[1])
                        bb := int(f64(100.) * rgb[2])
                        if rr < 10 {print(' ')} // FIXME : impl formatting width ?
                        print('$rr ')
                        if gg < 10 {print(' ')}
                        print('$gg ')
                        if bb < 10 {print(' ')}
                        print('$bb   ')
                }
                println('')
        }
}

fn main() {
        mut w := 10
        mut h := 10
        mut fnameout := ''
        mut arg := 1
        if arg < os.args.len {
                w = os.args[arg].int()
                arg++
                if arg < os.args.len {
                        h = os.args[arg].int()
                        arg++
                        if arg < os.args.len {
                                fnameout = os.args[arg]
                                arg++
                        }
                }
        }
        render(w, h, fnameout)
}
