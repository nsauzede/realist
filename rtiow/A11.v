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
	dielectric
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
	fuzz f32 = 0.
}

struct MDielectric {
	mtype MType = MType.dielectric
	ref_idx f32
}

union Material {
	generic MGeneric
	lambertian MLambertian
	metal MMetal
	dielectric MDielectric
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
		mut temp := (-b - math.sqrt(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vec.div(rec.p - s.center, s.radius)
//			eprintln('mat type ${s.material.mtype}')
			rec.mat = s.material
			return true
		}
		temp = (-b + math.sqrt(discriminant)) / a
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
//	mut temp_rec := HitRec{mat:0}
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

fn random_in_unit_sphere() vec.Vec3 {
	mut p := vec.Vec3{}
	for {
		p = vec.mult(2, vec.Vec3{random_double(), random_double(), random_double()}) - vec.Vec3{1,1,1}
		if p.squared_length() < 1.0 {
			break
		}
	}
	return p
}

fn (l MLambertian) scatter(r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	target := rec.p + rec.normal + random_in_unit_sphere()
	*scattered = ray.Ray{rec.p, target - rec.p}
	*attenuation = l.albedo
	return true
}

fn (m MMetal) scatter(r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	reflected := r_in.direction().unit_vector().reflect(rec.normal)
	*scattered = ray.Ray{rec.p, reflected + vec.mult(m.fuzz, random_in_unit_sphere())}
	*attenuation = m.albedo
	return scattered.direction().dot(rec.normal) > 0
}

fn schlick(cosine f32, ref_idx f32) f32 {
	mut r0 := (1. - ref_idx) / (1. + ref_idx)
	r0 = r0 * r0
	return r0 + (1. - r0) * math.pow(1. - cosine, 5)
}

fn (d MDielectric) scatter(r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
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
	if random_double() < reflect_prob {
		*scattered = ray.Ray{rec.p, reflected}
	} else {
		*scattered = ray.Ray{rec.p, refracted}
	}
	return true
}

fn (m Material) scatter(r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	if m.generic.mtype == .lambertian {
		return m.lambertian.scatter(r_in, rec, mut attenuation, mut scattered)
	} else if m.generic.mtype == .metal {
		return m.metal.scatter(r_in, rec, mut attenuation, mut scattered)
	} else if m.generic.mtype == .dielectric {
		return m.dielectric.scatter(r_in, rec, mut attenuation, mut scattered)
	} else {
		panic('unknown material ${m.metal.mtype}')
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
		t := .5 * (unit_direction.y + 1.)
		return vec.mult(1. - t, vec.Vec3{1., 1., 1.}) + vec.mult(t, vec.Vec3{.5, .7, 1.})
	}
}

fn random_double() f32 {
	return f32(rand.next(C.RAND_MAX)) / (f32(C.RAND_MAX) + 1.)
}

struct Camera {
	origin vec.Vec3
	lower_left_corner vec.Vec3
	horizontal vec.Vec3
	vertical vec.Vec3
}

fn new_camera(lookfrom vec.Vec3, lookat vec.Vec3, vup vec.Vec3, vfov f32, aspect f32) Camera {
	// vfov is top to bottom in degrees
	theta := vfov * math.pi / 180.
	half_height := math.tan(theta / 2.)
	half_width := aspect * half_height
	w := (lookfrom - lookat).unit_vector()
	u := vup.cross(w).unit_vector()
	v := w.cross(u)
	return Camera {
		lower_left_corner: lookfrom - vec.mult(half_width, u) - vec.mult(half_height, v) - w
		horizontal: vec.mult(2. * half_width, u)
		vertical: vec.mult(2. * half_height, v)
		origin: lookfrom
	}
}

fn (c Camera) get_ray(s f32, t f32) ray.Ray {
	return ray.Ray {
		c.origin,
		c.lower_left_corner
			+ vec.mult(s, c.horizontal)
			+ vec.mult(t, c.vertical)
			- c.origin
	}
}

fn main() {
	rand.seed(0)
	nx := 200
	ny := 100
	ns := 100
	println('P3') println('$nx $ny') println(255)
	cam := new_camera(
		vec.Vec3{-2., 2., 1.},
		vec.Vec3{0, 0, -1.},
		vec.Vec3{0, 1., 0},
		30, f32(nx) / f32(ny))
	R := math.cos(math.pi / 4.)
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
                }),
/*
		Hittable(HSphere{center: vec.Vec3{-R, 0, -1}, radius: R,
			material: Material(
				MLambertian{albedo: vec.Vec3{0, 0, 1}})
		}),
		Hittable(HSphere{center: vec.Vec3{R, 0, -1}, radius: R
			material: Material(
				MLambertian{albedo: vec.Vec3{1, 0, 0}})
		}),
*/
	]
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			mut col := vec.Vec3{0,0,0}
			for s := 0; s < ns; s++ {
				u := (f32(i) + random_double()) / f32(nx)
				v := (f32(j) + random_double()) / f32(ny)
				r := cam.get_ray(u, v)
				col = col + world.color(r, 0)
			}
			col = vec.div(col, ns)
			// Gamma 2 correction (square root)
			col = vec.Vec3{math.sqrt(col.x),math.sqrt(col.y),math.sqrt(col.z)}
			ir := int(255.99 * col.x)
			ig := int(255.99 * col.y)
			ib := int(255.99 * col.z)
			println('$ir $ig $ib')
		}
	}
}
