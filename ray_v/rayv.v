/*
 * Copyright(c) 2016-2019 Nicolas Sauzede (nsauzede@laposte.net)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
module main

import (
        vec
        os
        math
        strings
)

struct RgbColor{
	r f64
	g f64
	b f64
}

struct Thing {
	points []f64
}

struct Scene {
mut:
	objects []Thing
}

const (
  math_MaxFloat64 = 9999999999999999999.
  init_t12 = [f64(0.); 2]
  init_omin = [f64(0.); 7]
  init_space = ' '
  black_color = RgbColor{ f64(0.), f64(0.), f64(0.) }
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

fn intersec(s &f64, o, v vec.Vector) f64 {
        mut t := f64(0.)
        center := vec.Vector{s[0], s[1], s[2]}
        rad := s[3]
        vt := o.sub(center)
        a := v.dot(v)
        b := 2. * v.dot(vt)
        c := vt.dot(vt) - rad * rad
        mut t12 := init_t12
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

fn trace(scene &Scene, o, v vec.Vector) RgbColor {
//        tmin := math.MaxFloat64
        mut tmin := math_MaxFloat64
        mut omin := init_omin       // FIXME : needs to be big enough for any obj
        for s in scene.objects {
                t := intersec(s.points.data, o, v)
                if t > 0 && t < tmin {
                        tmin = t
                        omin = s.points
                }
        }
        if tmin < math_MaxFloat64 {
                return RgbColor{ omin[4], omin[5], omin[6] }
        }
        return black_color
}

fn render(w, h int, fnameout string) {
        ratiox := 1.
        ratioy := 1.
        ww := 1. * ratiox
        hh := ww * f64(h) / f64(w) * ratioy
        e := vec.Vector{0.4, 0, 0.4}
        mut f := vec.Vector{-1, 0, -1}
        mut u := vec.Vector{-0.707107, 0, 0.707107}

        mut scene := Scene{}
        scene.objects << Thing{ [f64(0), -0.1, 0, 0.05, 0.8, 0.8, 0.8] }
        scene.objects << Thing{ [f64(0), 0, 0, 0.05,   0.8, 0.8, 0.8] }
        scene.objects << Thing{ [f64(0), 0.1, 0,   0.05,   0.8, 0.8, 0.8] }
        scene.objects << Thing{ [f64(0.1), -0.05, 0,   0.05,   0.8, 0, 0] }
        scene.objects << Thing{ [f64(0.1), 0.05, 0,    0.05,   0, 0, 0.8] }
        scene.objects << Thing{ [f64(0.2), 0, 0,   0.05,   0, 0.8, 0] }
        scene.objects << Thing{ [f64(0.05), -0.05, 0.1,    0.05,   0.8, 0.8, 0.8] }
        scene.objects << Thing{ [f64(0.05), 0.05, 0.1, 0.05,   0.8, 0.8, 0.8] }
        scene.objects << Thing{ [f64(0), -0.5, 0.5,    0.02,   1, 1, 0] }

        u.normalize()
        mut r := f.cross(u)
        u = r.cross(f)
        u.normalize()
        r.normalize()

        mut picturestring := strings.new_builder(1024)
        picturestring.writeln('P3')
        picturestring.writeln('$w $h')
        picturestring.writeln('100')
        for j := 0; j < h; j++ {
                vu := u.mult((f64(h) - f64(j) - 1 - f64(h) / 2) / f64(h) * hh)

                for i := 0; i < w; i++ {
                        vr := r.mult((f64(i) - f64(w) / 2) / f64(w) * ww)
                        mut v := f.add(vu.add( vr))
                        v.normalize()
                        rgb := trace(scene, e, v)
                        rr := int(f64(100.) * rgb.r)
                        gg := int(f64(100.) * rgb.g)
                        bb := int(f64(100.) * rgb.b)
                        if rr < 10 {picturestring.write(init_space)} // FIXME : impl formatting width ?
                        picturestring.write('$rr ')
                        if gg < 10 {picturestring.write(init_space)}
                        picturestring.write('$gg ')
                        if bb < 10 {picturestring.write(init_space)}
                        picturestring.write('$bb   ')
                }
                picturestring.writeln('')
        }
        
        if fnameout == '' {
                print( picturestring.str() )
        }else{
                os.write_file( fnameout, picturestring.str() )
        }
        
        picturestring.free()
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
