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

struct RgbColor {
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
	HUGE_VAL = f64(C.HUGE_VAL)
	math_MaxFloat64 = 1.797693134862315708145274237317043567981e+308
	init_t12 = [f64(0.)].repeat(2)
	init_omin = [f64(0.)].repeat(7) // WARNING : needs to be big enough for any obj
	init_space = ' '
	black_color = RgbColor {
		f64(0.),f64(0.),f64(0.)}
)

fn solve_tri(a, b, c f64, t mut []f64) int {
	mut sol := 0
	d := b * b - f64(4.) * a * c
	if d > 0 {
		sd := math.sqrt(d)
		t[0] = (-b - sd) / 2 / a
		t[1] = (-b + sd) / 2 / a
		sol = 2
	}
	else if d == 0 {
		t[0] = -b / 2 / a
		sol = 1
	}
	return sol
}

fn intersec(s &f64, o, v vec.Vector) f64 {
	mut t := HUGE_VAL
	center := vec.Vector {
		s[0],s[1],s[2]}
	rad := s[3]
	vt := o.sub(center)
	a := v.dot(v)
	b := f64(2.) * v.dot(vt)
	c := vt.dot(vt) - rad * rad
	mut t12 := init_t12
	sol := solve_tri(a, b, c, mut t12)
	if sol == 2 {
		if t12[0] < t12[1] {
			t = t12[0]
		}
		else {
			t = t12[1]
		}
	}
	else if sol == 1 {
		t = t12[0]
	}
	return t
}

fn trace(scene &Scene, o, v vec.Vector) RgbColor {
	mut tmin := HUGE_VAL
	mut omin := init_omin
	for s in scene.objects {
		t := intersec(s.points.data, o, v)
		if t > 0 && t < tmin {
			tmin = t
			omin = s.points
		}
	}
	if tmin < HUGE_VAL {
		return RgbColor {
			omin[4],omin[5],omin[6]}
	}
	return black_color
}

fn render(w, h int, fnameout string) {
	ratiox := f64(1.)
	ratioy := f64(1.)
	ww := ratiox
	hh := ww * f64(h) / f64(w) * ratioy
	e := vec.Vector {
		f64(0.4),f64(0),f64(0.4)}
	/*mut*/
	f := vec.Vector {
		f64(-1),f64(0),f64(-1)}
	mut u := vec.Vector {
		f64(-0.707107),f64(0),f64(0.707107)}
	mut scene := Scene{}
	scene.objects << Thing{
		[f64(0), f64(-0.1), f64(0), f64(0.05), f64(0.8), f64(0.8), f64(0.8)]}
	scene.objects << Thing{
		[f64(0), f64(0), f64(0), f64(0.05), f64(0.8), f64(0.8), f64(0.8)]}
	scene.objects << Thing{
		[f64(0), f64(0.1), f64(0), f64(0.05), f64(0.8), f64(0.8), f64(0.8)]}
	scene.objects << Thing{
		[f64(0.1), f64(-0.05), f64(0), f64(0.05), f64(0.8), f64(0), f64(0)]}
	scene.objects << Thing{
		[f64(0.1), f64(0.05), f64(0), f64(0.05), f64(0), f64(0), f64(0.8)]}
	scene.objects << Thing{
		[f64(0.2), f64(0), f64(0), f64(0.05), f64(0), f64(0.8), f64(0)]}
	scene.objects << Thing{
		[f64(0.05), f64(-0.05), f64(0.1), f64(0.05), f64(0.8), f64(0.8), f64(0.8)]}
	scene.objects << Thing{
		[f64(0.05), f64(0.05), f64(0.1), f64(0.05), f64(0.8), f64(0.8), f64(0.8)]}
	scene.objects << Thing{
		[f64(0), f64(-0.5), f64(0.5), f64(0.02), f64(1), f64(1), f64(0)]}
	u.normalize()
	mut r := f.cross(u)
	u = r.cross(f)
	u.normalize()
	r.normalize()
	max := 255
	// we create the output file unconditionally, before checking if fnameout
	// is empty. This is due to a bug where mut os.File reassigned by os.create
	// in sub-block seems to be wiped at block exit (??)
//	mut fout := os.create(fnameout) or { exit }
	mut fout := os.create(fnameout) or { os.File{cfile: 0} }
	mut bytes := byteptr(0)
	mut nbytes := 0
	mut picture_string := strings.new_builder(1024)
	if fnameout != '' {
		fout.writeln('P6')
		nbytes = 3 * h * w
		bytes = malloc(nbytes)
		fout.writeln('$w $h')
		fout.writeln('$max')
	} else {
		picture_string.writeln('P3')
		picture_string.writeln('$w $h')
		picture_string.writeln('$max')
	}
	for j := 0; j < h; j++ {
		vu := u.mult((f64(h) - f64(j) - 1 - f64(h) / 2) / f64(h) * hh)
		for i := 0; i < w; i++ {
			vr := r.mult((f64(i) - f64(w) / 2) / f64(w) * ww)
			mut v := f.add(vu.add(vr))
			v.normalize()
			rgb := trace(scene, e, v)
			rr := int(f64(max) * rgb.r)
			gg := int(f64(max) * rgb.g)
			bb := int(f64(max) * rgb.b)
			if fnameout != '' {
				bytes[(j * w + i) * 3 + 0] = rr
				bytes[(j * w + i) * 3 + 1] = gg
				bytes[(j * w + i) * 3 + 2] = bb
			} else {
				picture_string.write('${rr:2d} ${gg:2d} ${bb:2d}   ')
			}
		}
		if fnameout == '' {
			picture_string.writeln('')
		}
	}
	if fnameout == '' {
		print(picture_string.str())
	} else {
		fout.write_bytes(bytes, nbytes)
		fout.close()
		unsafe {
			free(bytes)
		}
	}
	picture_string.free()
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
