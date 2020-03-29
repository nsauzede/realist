#!/bin/env -S v run

module main

import vec
import ray
import math
import rand

enum HType {
	sphere
}
struct HSphere {
	center vec.Vec3
	radius f32
}
union HData {
	sphere HSphere
}

struct Hittable {
	htype HType
	data HData
}

struct HitRec {
mut:
	t f32			// hit time
	p vec.Vec3		// hit point coords
	normal vec.Vec3		// normal at hit point
}

fn (s HSphere) hit(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	oc := r.origin() - s.center
	a := r.direction().dot(r.direction())
	b := oc.dot(r.direction())
//	c := oc.dot(oc) - s.radius * s.radius
	ra := s.radius * s.radius
	c := oc.dot(oc) - ra
	discriminant := b * b - a * c
//	println('ray=${r}\noc=$oc\na=$a b=$b ra=$ra c=$c d=$discriminant')
/*
	println('sc=${s.center}')
	println('ray=${r}')
	println('oc=$oc')
	println('a=$a')
	println('b=$b')
	println('ra=$ra')
	println('c=$c')
	println('d=$discriminant')
*/
	if discriminant > 0 {
		mut temp := (-b - math.sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vec.div(rec.p - s.center, s.radius)
			return true
		}
		temp = (-b + math.sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vec.div(rec.p - s.center, s.radius)
			return true
		}
	}
	return false
}

fn (h Hittable) hit(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	if h.htype == .sphere {
		return h.data.sphere.hit(r, t_min, t_max, mut rec)
	}
	return false
}

fn (hh []Hittable) hit(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
//	println('max=$t_max')
	mut temp_rec := HitRec{}
	mut hit_anything := false
	mut closest_so_far := t_max
	for h in hh {
		if h.hit(r, t_min, closest_so_far, mut temp_rec) {
			hit_anything = true
			closest_so_far = temp_rec.t
			*rec = temp_rec
		}
	}
	return hit_anything
}

fn (s HSphere) make() Hittable {
	return Hittable {
		htype:.sphere
		data:HData{
			sphere: s
		}
	}
}

fn random_in_unit_sphere() vec.Vec3 {
	mut p := vec.Vec3{}
	for {
		r1 := random_f()
		r2 := random_f()
		r3 := random_f()
		p = vec.mult(2, vec.Vec3{r1, r2, r3}) - vec.Vec3{1,1,1}
		if p.squared_length() < 1.0 {
			break
		}
	}
	return p
}

fn (world []Hittable) color(r ray.Ray) vec.Vec3 {
	mut rec := HitRec{}
	max := math.max_f32
//	println('max=$max')
//	if world.hit(r, 0, max, mut rec) {
	if world.hit(r, 0.001, max, mut rec) {
//		println('HIT')
//		println('nrm=${rec.normal}')
		target := rec.normal + random_in_unit_sphere()
		return vec.mult(0.5, world.color(ray.Ray{rec.p, target}))
	} else {
//		println('NOT HIT')
		unit_direction := r.direction().unit_vector()
		t := .5 * (unit_direction.y + 1.)
		return vec.mult(1. - t, vec.Vec3{1., 1., 1.}) + vec.mult(t, vec.Vec3{.5, .7, 1})
	}
}

fn random_f() f32 {
//	return f32(rand.next(C.RAND_MAX)) / (f32(C.RAND_MAX) + 1.)
	r := f32(rand.next(C.RAND_MAX)) / (f32(C.RAND_MAX) + f32(1))
//	print('r=$r ')
	return r
}

struct Camera {
	lower_left_corner vec.Vec3
	horizontal vec.Vec3
	vertical vec.Vec3
	origin vec.Vec3
}

fn (c Camera) get_ray(u f32, v f32) ray.Ray {
	return ray.Ray {
		c.origin,
		c.lower_left_corner
		+ vec.mult(u, c.horizontal)
		+ vec.mult(v, c.vertical)
		- c.origin
	}
/*
	uv := vec.Vec3{u, v, 0}
	println('uv=$uv')
	r := ray.Ray {
		c.origin,
		c.lower_left_corner
		+ vec.mult(u, c.horizontal)
		+ vec.mult(v, c.vertical)
		- c.origin
	}
	println('r=$r')
	return r
*/
}

fn main() {
	rand.seed(0)
	nx := 200
	ny := 100
	ns := 100
	println('P3') println('$nx $ny') println(255)
	cam := Camera {
		lower_left_corner : vec.Vec3 {-2., -1., -1.}
		horizontal : vec.Vec3 {4., 0., 0.}
		vertical : vec.Vec3 {0., 2., 0.}
		origin : vec.Vec3 {0., 0., 0.}
	}
//	println(cam)
	world := [
		HSphere{center: vec.Vec3{0, 0, -1}, radius: .5}.make(),
		HSphere{center: vec.Vec3{0, -100.5, -1}, radius: 100}.make()
	]
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			mut col := vec.Vec3{0,0,0}
			for s := 0; s < ns; s++ {
//				println('rfcnt=$rfcnt riuscnt=$riuscnt riudcnt=$riudcnt')
				u := (f32(i) + random_f()) / f32(nx)
				v := (f32(j) + random_f()) / f32(ny)
//				println('u=$u v=$v;')
//				uv := vec.Vec3{u, v, 0}
//				println('uv=$uv')
				r := cam.get_ray(u, v)
//				println('r=$r')
				col = col + world.color(r)
//				println('rfcnt=$rfcnt riuscnt=$riuscnt riudcnt=$riudcnt')
			}
			col = vec.div(col, ns)
			ir := int(f32(255.99) * col.x)
			ig := int(f32(255.99) * col.y)
			ib := int(f32(255.99) * col.z)
			println('$ir $ig $ib')
		}
	}
}
