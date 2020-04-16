#!/bin/env -S v run

module main

import vec
import ray
import math
import rand

enum HType {
	sphere
}

enum MType {
	lambertian
	metal
}

struct MGeneric {
	mtype MType
}

struct MLambertian {
	mtype MType = MType.lambertian
	albedo vec.Vec3
}

struct MMetal {
	mtype MType = MType.metal
	albedo vec.Vec3
}

union Material {
	generic MGeneric
	lambertian MLambertian
	metal MMetal
}

struct HGeneric {
	htype HType
}

struct HSphere {
	htype HType = HType.sphere
	center vec.Vec3
	radius f32
	material Material
}

union Hittable {
	generic HGeneric
	sphere HSphere
}

struct HitRec {
mut:
	t f32			// hit time
	p vec.Vec3		// hit point coords
	normal vec.Vec3		// normal at hit point
	// mat should be a ref ! (but does not work)
	mat Material		// material at hit point
}

fn (s HSphere) hit(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	oc := r.origin() - s.center
	a := r.direction().dot(r.direction())
	b := oc.dot(r.direction())
	c := oc.dot(oc) - s.radius * s.radius
	discriminant := b * b - a * c
	if discriminant > 0 {
		mut temp := (-b - math.sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vec.div(rec.p - s.center, s.radius)
//			eprintln('mat type ${s.material.mtype}')
			rec.mat = s.material
			return true
		}
		temp = (-b + math.sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vec.div(rec.p - s.center, s.radius)
//			eprintln('mat type ${s.material.mtype}')
			rec.mat = s.material
			return true
		}
	}
	return false
}

fn (h Hittable) hit(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	if h.generic.htype == .sphere {
		return h.sphere.hit(r, t_min, t_max, mut rec)
	}
	return false
}

fn (hh []Hittable) hit(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	mut hit_anything := false
	mut closest_so_far := t_max
	for h in hh {
		if h.hit(r, t_min, closest_so_far, rec) {
			hit_anything = true
			closest_so_far = rec.t
		}
	}
	return hit_anything
}

fn random_in_unit_sphere() vec.Vec3 {
	mut p := vec.Vec3{}
	for {
		r1 := random_f() r2:= random_f() r3 := random_f()
		p = vec.mult(2, vec.Vec3{r1, r2, r3}) - vec.Vec3{1,1,1}
		if p.squared_length() < 1.0 {
			break
		}
	}
	return p
}

fn (m Material) scatter(r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	if m.generic.mtype == .lambertian {
		target := rec.normal + random_in_unit_sphere()
		// workaround buggy compiler
		{*scattered = ray.Ray{rec.p, target}}
		*attenuation = m.lambertian.albedo
//		eprintln('Hello !!!!!!! lambertian ${m.lambertian.mtype}')
		return true
	} else {
		reflected := r_in.direction().unit_vector().reflect(rec.normal)
		// workaround buggy compiler
		{*scattered = ray.Ray{rec.p, reflected}}
		*attenuation = m.metal.albedo
//		eprintln('Hello !!!!!!! non-lambertian ${m.metal.mtype}')
		return scattered.direction().dot(rec.normal) > 0
	}
}

fn (world []Hittable) color(r ray.Ray, depth int) vec.Vec3 {
//	mut rec := HitRec{mat:0}
	mut rec := HitRec{}
	// remove acne by starting at 0.001
	if world.hit(r, 0.001, math.max_f32, mut rec) {
		mut scattered := ray.Ray{}
		mut attenuation := vec.Vec3{}
		if depth < 50 && rec.mat.scatter(r, rec, mut attenuation, mut scattered) {
			return attenuation * world.color(scattered, depth + 1)
		} else {
			return vec.Vec3{0,0,0}
		}
	} else {
		unit_direction := r.direction().unit_vector()
		t := .5 * (unit_direction.y + 1.0)
		return vec.mult(1.0 - t, vec.Vec3{1, 1, 1}) + vec.mult(t, vec.Vec3{.5, .7, 1})
	}
}

fn random_f() f32 {
	return f32(rand.next(C.RAND_MAX)) / (f32(C.RAND_MAX) + 1.0)
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
c.lower_left_corner + vec.mult(u, c.horizontal) + vec.mult(v, c.vertical) - c.origin
	}
}

fn main() {
	rand.seed(0)
	nx := 200
	ny := 100
	ns := 100
	println('P3') println('$nx $ny') println(255)
	cam := Camera {
		lower_left_corner : vec.Vec3 {-2, -1, -1}
		horizontal : vec.Vec3 {4, 0, 0}
		vertical : vec.Vec3 {0, 2, 0}
		origin : vec.Vec3 {0, 0, 0}
	}
	world := [
		Hittable(HSphere{
			center: vec.Vec3{0, 0, -1}, radius: 0.5,
			material: Material(
				MLambertian{albedo: vec.Vec3{0.8, 0.3, 0.3}})
		}),
		Hittable(HSphere{center: vec.Vec3{0, -100.5, -1}, radius: 100
			material: Material(
				MLambertian{albedo: vec.Vec3{0.8, 0.8, 0.0}})
		}),
		Hittable(HSphere{center: vec.Vec3{1, 0, -1}, radius: 0.5
			material: Material(
				MMetal{albedo: vec.Vec3{0.8, 0.6, 0.2}})
		}),
		Hittable(HSphere{center: vec.Vec3{-1, 0, -1}, radius: 0.5
			material: Material(
				MMetal{albedo: vec.Vec3{0.8, 0.8, 0.8}})
		})
	]
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			mut col := vec.Vec3{0,0,0}
			for s := 0; s < ns; s++ {
				u := (f32(i) + random_f()) / f32(nx)
				v := (f32(j) + random_f()) / f32(ny)
				r := cam.get_ray(u, v)
				col = col + world.color(r, 0)
			}
			col = vec.div(col, ns)
			// Gamma 2 correction (square root)
			col = vec.Vec3{math.sqrtf(col.x),math.sqrtf(col.y),math.sqrtf(col.z)}
			ir := int(f32(255.99) * col.x)
			ig := int(f32(255.99) * col.y)
			ib := int(f32(255.99) * col.z)
			println('$ir $ig $ib')
		}
	}
}
