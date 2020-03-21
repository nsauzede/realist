#!/bin/env -S v run

module main

import vec
import ray
import math
import rand

//__global rfcnt int
//__global riuscnt int
//__global riudcnt int

struct HitRec {
mut:
	t f32			// hit time
	p vec.Vec3		// hit point coords
	normal vec.Vec3		// normal at hit point
	ph voidptr		// hittable at hit point (material, etc..)
}

fn random_f() f32 {
//	rfcnt++
	return f32(rand.next(C.RAND_MAX)) / (f32(C.RAND_MAX) + 1.)
}

fn random_in_unit_sphere() vec.Vec3 {
//	riuscnt++
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

enum HType {
	sphere
}

type ScatterCallback fn(obj voidptr, r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool

struct MGeneric {
	scattercb ScatterCallback
}

struct MLambertian {
	scattercb ScatterCallback = ScatterCallback(cb_scatter_lambertian)
	albedo vec.Vec3
}

struct MMetal {
	scattercb ScatterCallback = ScatterCallback(cb_scatter_metal)
	albedo vec.Vec3
	fuzz f32 = f32(0.)
}

struct MDielectric {
	scattercb ScatterCallback = ScatterCallback(cb_scatter_dielectric)
	ref_idx f32
}

union Material {
	generic MGeneric
	lambertian MLambertian
	metal MMetal
	dielectric MDielectric
}

type StringCallback fn(obj voidptr) string
type HitCallback fn(obj voidptr, r ray.Ray, t_min f32, closest f32, rec mut HitRec) bool

struct HGeneric {
	strcb StringCallback
	hitcb HitCallback
	material Material
}

struct HSphere {
	strcb StringCallback = StringCallback(cb_sphere_str)
	hitcb HitCallback = HitCallback(cb_sphere_hit)
	material Material
	center vec.Vec3
	radius f32
}

union Hittable {
	generic HGeneric
	sphere HSphere
}

fn cb_sphere_str(obj voidptr) string {
	s := &HSphere(obj) return '{S:$s.center,$s.radius}'
}

fn cb_sphere_hit(obj voidptr, r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	s := &HSphere(obj)
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
			rec.ph = obj
			return true
		}
		temp = (-b + math.sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vec.div(rec.p - s.center, s.radius)
			rec.ph = obj
			return true
		}
	}
	return false
}

fn (hh []Hittable) hit(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	mut hit_anything := false
	mut closest_so_far := t_max
	for i, h in hh {
		if h.generic.hitcb(&hh[i], r, t_min, closest_so_far, mut rec) {
			hit_anything = true
			closest_so_far = rec.t
		}
	}
	return hit_anything
}

fn cb_scatter_lambertian(obj voidptr, r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	l := &MLambertian(obj)
	target := rec.normal + random_in_unit_sphere()
	*scattered = ray.Ray{rec.p, target}
	*attenuation = l.albedo
	return true
}

fn cb_scatter_metal(obj voidptr, r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	m := &MMetal(obj)
	reflected := r_in.direction().unit_vector().reflect(rec.normal)
	*scattered = ray.Ray{rec.p, reflected + vec.mult(m.fuzz, random_in_unit_sphere())}
	*attenuation = m.albedo
	return scattered.direction().dot(rec.normal) > 0
}

fn schlick(cosine f32, ref_idx f32) f32 {
	mut r0 := (1. - ref_idx) / (1. + ref_idx)
	r0 = r0 * r0
	return r0 + (1. - r0) * math.powf(1. - cosine, 5)
}

fn cb_scatter_dielectric(obj voidptr, r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	d := &MDielectric(obj)
	mut outward_normal := vec.Vec3{}
	reflected := r_in.direction().reflect(rec.normal)
	mut ni_over_nt := f32(0)
	*attenuation = vec.Vec3{1., 1., 1.}
	mut refracted := vec.Vec3{}
	mut reflect_prob := f32(1)
	mut cosine := f32(0)
	dot := r_in.direction().dot(rec.normal)
	len := r_in.direction().length()
	if dot > 0 {
		outward_normal = vec.mult(-1., rec.normal)
		ni_over_nt = d.ref_idx
		cosine = d.ref_idx * dot / len
	} else {
		outward_normal = rec.normal
		ni_over_nt = 1. / d.ref_idx
		cosine = -dot / len
	}
	if r_in.direction().refract(outward_normal, ni_over_nt, mut refracted) {
		reflect_prob = schlick(cosine, d.ref_idx)
	}
	if random_f() < reflect_prob {
		*scattered = ray.Ray{rec.p, reflected}
	} else {
		*scattered = ray.Ray{rec.p, refracted}
	}
	return true
}

fn (world []Hittable) color(r ray.Ray, depth int) vec.Vec3 {
	mut rec := HitRec{ph:0}
	// remove acne by starting at 0.001
	if world.hit(r, 0.001, math.max_f32, mut rec) {
//		println('HIT')
		mut scattered := ray.Ray{}
		mut attenuation := vec.Vec3{}
		h := &Hittable(rec.ph)
		if depth < 50 && h.generic.material.generic.scattercb(h.generic.material.generic, r, rec, mut attenuation, mut scattered) {
//		println('ATT')
			return attenuation * world.color(scattered, depth + 1)
		} else {
//		println('NOT ATT')
			return vec.Vec3{0,0,0}
		}
	} else {
//		println('NOT HIT')
		unit_direction := r.direction().unit_vector()
		t := .5 * (unit_direction.y + 1.)
		return vec.mult(1. - t, vec.Vec3{1., 1., 1.}) + vec.mult(t, vec.Vec3{.5, .7, 1.})
	}
}

struct Camera {
mut:
	lower_left_corner vec.Vec3
	horizontal vec.Vec3
	vertical vec.Vec3
	origin vec.Vec3
}

fn (cam mut Camera) make(lookfrom vec.Vec3, lookat vec.Vec3, vup vec.Vec3, vfov f32, aspect f32) {
//	println(lookfrom)
	theta := vfov * math.pi / 180.
	half_height := math.tanf(theta / 2.)
	half_width := aspect * half_height

	w := (lookfrom - lookat).unit_vector()
	u := vup.cross(w).unit_vector()
	v := w.cross(u)

	cam.lower_left_corner = lookfrom
		- vec.mult(half_width, u)
		- vec.mult(half_height, v)
		- w
	cam.horizontal = vec.mult(2. * half_width, u)
	cam.vertical = vec.mult(2. * half_height, v)
	cam.origin = lookfrom
}

fn (c Camera) get_ray(s f32, t f32) ray.Ray {
	return ray.Ray {
		c.origin,
		vec.mult(t, c.vertical)
		+ vec.mult(s, c.horizontal)
		+ c.lower_left_corner 
		- c.origin
	}
}

pub fn (h Hittable) str() string {
	return h.generic.strcb(h)
}

fn main() {
	rand.seed(0)
	nx := 200
	ny := 100
	ns := 100
	println('P3') println('$nx $ny') println(255)
	mut cam := Camera{}
	cam.make(vec.Vec3{-2,2,1}, vec.Vec3{0,0,-1}, vec.Vec3{0,1,0}, 90., f32(nx)/f32(ny))
	world := [
		Hittable(HSphere{
			center: vec.Vec3{0, 0, -1}, radius: 0.5,
			material: Material(
				MLambertian{albedo: vec.Vec3{0.1, 0.2, 0.5}})
		}),
		Hittable(HSphere{center: vec.Vec3{0, -100.5, -1}, radius: 100
			material: Material(
				MLambertian{albedo: vec.Vec3{0.8, 0.8, 0.0}})
		}),
		Hittable(HSphere{center: vec.Vec3{1, 0, -1}, radius: 0.5
			material: Material(
				MMetal{albedo: vec.Vec3{0.8, 0.6, 0.2}, fuzz: 0.3})
		}),
		Hittable(HSphere{center: vec.Vec3{-1, 0, -1}, radius: 0.5
			material: Material(
				MDielectric{ref_idx: 1.5})
		}),
		Hittable(HSphere{center: vec.Vec3{-1, 0, -1}, radius: -0.45
			material: Material(
				MDielectric{ref_idx: 1.5})
		})
	]
//	println(cam)
//	println(world)
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			mut col := vec.Vec3{0,0,0}
			for s := 0; s < ns; s++ {
//				println('rfcnt=$rfcnt riuscnt=$riuscnt riudcnt=$riudcnt')
				u := (f32(i) + random_f()) / f32(nx)
				v := (f32(j) + random_f()) / f32(ny)
//				println('u=$u v=$v rfcnt=$rfcnt riuscnt=$riuscnt')
//				uv := vec.Vec3{u, v, 0}
//				println('uv=$uv')
				r := cam.get_ray(u, v)
//				println('j=$j i=$i s=$s r=$r')
				col = col + world.color(r, 0)
			}
			col = vec.div(col, ns)
			// Gamma 2 correction (square root)
			col = vec.Vec3{math.sqrtf(col.x),math.sqrtf(col.y),math.sqrtf(col.z)}
			ir := int(255.99 * col.x)
			ig := int(255.99 * col.y)
			ib := int(255.99 * col.z)
			println('$ir $ig $ib')
		}
	}
}
