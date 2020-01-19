#!/bin/env -S v run

module main

import vec
import ray
import math
import rand
import os
import strings

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

struct HitRec {
mut:
	t f32			// hit time
	p vec.Vec3		// hit point coords
	normal vec.Vec3		// normal at hit point
	// mat should be a ref ! (but does not work)
	mat Material		// material at hit point
}

type ScatterCallback fn(obj voidptr, r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool

type HitCallback fn(obj voidptr, r ray.Ray, t_min f32, closest f32, rec mut HitRec) bool

struct HGeneric {
	htype HType
	hitcb HitCallback
	material Material
}

struct HSphere {
	htype HType = HType.sphere
	hitcb HitCallback = HitCallback(cb_sphere_hit)
	material Material
	center vec.Vec3
	radius f32
}

union Hittable {
	generic HGeneric
	sphere HSphere
}

fn (s HSphere) hit0(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
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

fn cb_sphere_hit(obj voidptr, r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	s := &HSphere(obj)
//	return s.hit(r, t_min, t_max, mut rec)
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

fn (hh []Hittable) hit0(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	mut temp_rec := HitRec{}
	mut hit_anything := false
	mut closest_so_far := t_max
	for h in hh {
		mut is_hit := false
		if h.generic.htype == .sphere {
			is_hit = h.sphere.hit0(r, t_min, closest_so_far, mut temp_rec)
		}
		if is_hit {
			hit_anything = true
			closest_so_far = temp_rec.t
			*rec = temp_rec
		}
	}
	return hit_anything
}

fn (hh []Hittable) hit1(r ray.Ray, t_min f32, t_max f32, rec mut HitRec) bool {
	mut hit_anything := false
	mut closest_so_far := t_max
	for h in hh {
		if h.generic.hitcb(h, r, t_min, closest_so_far, mut rec) {
			hit_anything = true
			closest_so_far = rec.t
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

fn cb_scatter_lambertian(obj voidptr, r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	l := &MLambertian(obj)
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
	if random_double() < reflect_prob {
		*scattered = ray.Ray{rec.p, reflected}
	} else {
		*scattered = ray.Ray{rec.p, refracted}
	}
	return true
}

fn (m Material) scatter0(r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
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

const (
	cbs_scatter = [
		ScatterCallback(cb_scatter_lambertian),
		ScatterCallback(cb_scatter_metal),
		ScatterCallback(cb_scatter_dielectric),
	]
)

fn (m Material) scatter1(r_in ray.Ray, rec HitRec, attenuation mut vec.Vec3, scattered mut ray.Ray) bool {
	cb_scatter := cbs_scatter[m.generic.mtype]
	return cb_scatter(&m, r_in, rec, mut attenuation, mut scattered)
}

fn (world []Hittable) color(r ray.Ray, depth int) vec.Vec3 {
//	mut rec := HitRec{mat:0}
	mut rec := HitRec{}
	// remove acne by starting at 0.001
	if world.hit1(r, 0.001, math.max_f32, mut rec) {
//	if world.hit0(r, 0.001, math.max_f32, mut rec) {
		mut scattered := ray.Ray{}
		mut attenuation := vec.Vec3{}
		if depth < 50 && rec.mat.scatter0(r, rec, mut attenuation, mut scattered) {
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

fn random_in_unit_disk() vec.Vec3 {
	mut p := vec.Vec3{}
	for {
		p = vec.mult(2, vec.Vec3{random_double(), random_double(), 0}) - vec.Vec3{1, 1, 0}
		if p.dot(p) < 1.0 {
			break
		}
	}
	return p
}

struct Camera {
	origin vec.Vec3
	lower_left_corner vec.Vec3
	horizontal vec.Vec3
	vertical vec.Vec3
	u vec.Vec3
	v vec.Vec3
	w vec.Vec3
	lens_radius f32
}

pub fn (c Camera) str() string {
	return '{origin = $c.origin, lower_left_corner = $c.lower_left_corner, 
	horizontal = $c.horizontal, vertical = $c.vertical, 
  u = $c.u, v = $c.v, w = $c.w, 
  lens_radius = $c.lens_radius'
}

fn new_camera(lookfrom vec.Vec3, lookat vec.Vec3, vup vec.Vec3, vfov f32, aspect f32, aperture f32, focus_dist f32) Camera {
	// vfov is top to bottom in degrees
	theta := vfov * math.pi / 180.
	half_height := math.tan(theta / 2.)
	half_width := aspect * half_height
	w := (lookfrom - lookat).unit_vector()
	u := vup.cross(w).unit_vector()
	v := w.cross(u)
	return Camera {
		lens_radius: aperture / 2.
		lower_left_corner: lookfrom
			- vec.mult(half_width * focus_dist, u)
			- vec.mult(half_height * focus_dist, v)
			- vec.mult(focus_dist, w)
		horizontal: vec.mult(2. * half_width * focus_dist, u)
		vertical: vec.mult(2. * half_height * focus_dist, v)
		origin: lookfrom
		u: u, v: v, w: w
	}
}

fn (c Camera) get_ray(s f32, t f32) ray.Ray {
	rd := vec.mult(c.lens_radius, random_in_unit_disk())
	offset := vec.mult(rd.x, c.u) + vec.mult(rd.y, c.v)
	return ray.Ray {
		c.origin + offset,
		c.lower_left_corner
			+ vec.mult(s, c.horizontal)
			+ vec.mult(t, c.vertical)
			- c.origin - offset
	}
}

fn new_world() []Hittable {
	mut world := []Hittable
	world << Hittable(HSphere{
		center: vec.Vec3{0, -1000, 0}, radius: 1000,
		material: Material(MLambertian{albedo: vec.Vec3{0.5, 0.5, 0.5}})
	})
	n := 11 //11
	for a := -n; a < n; a++ {
		for b := -n; b < n; b++ {
			choose_mat := random_double()
			r01 := random_double()
			r02 := random_double()
			center := vec.Vec3{
				f32(a)+0.9*r02,
				0.2,
				f32(b)+0.9*r01}
//			eprintln('a=$a b=$b center=$center')
			if (center - vec.Vec3{4,0.2,0}).length() > 0.9 {
				if choose_mat < 0.8 {  // diffuse
					r1 := random_double()
					r2 := random_double()
					r3 := random_double()
					r4 := random_double()
					r5 := random_double()
					r6 := random_double()
world << Hittable(HSphere{
	center: center, radius: 0.2,
	material: Material(MLambertian{albedo: vec.Vec3{
		r6*r5,r4*r3,r2*r1}})
})
				} else if choose_mat < 0.95 { // metal
					r1 := random_double()
					r2 := random_double()
					r3 := random_double()
					r4 := random_double()
world << Hittable(HSphere{
	center: center, radius: 0.2
	material: Material(MMetal{albedo: vec.Vec3{
		0.5 * (1. + r4),
		0.5 * (1. + r3),
		0.5 * (1. + r2)},
		fuzz: 0.5 * r1})
})
				} else {  // glass
world << Hittable(HSphere{
	center: center, radius: 0.2
	material: Material(MDielectric{ref_idx: 1.5})
})
				}
			}
		}
	}
	world <<
		Hittable(HSphere{center: vec.Vec3{-4, 1, 0}, radius: 1
                        material: Material(
                                MLambertian{albedo: vec.Vec3{0.4, 0.2, 0.1}})
                })
	world <<
                Hittable(HSphere{center: vec.Vec3{0, 1, 0}, radius: 1
                        material: Material(
                                MDielectric{ref_idx: 1.5})
                })
	world <<
                Hittable(HSphere{center: vec.Vec3{4, 1, 0}, radius: 1
                        material: Material(
                                MMetal{albedo: vec.Vec3{0.7, 0.6, 0.5}, fuzz: 0.0})
                })
	return world
}

fn main() {
	rand.seed(0)
	mut fnameout := ''
	mut arg := 1
	if arg < os.args.len {
		fnameout = os.args[arg++]
	}
	// clean next line condition when sub block os.create bug is fixed
	mut fout := os.create(fnameout) or { exit }
	mut bytes := byteptr(0)
	mut nbytes := 0
//	nx := 200 ny := 100 ns := 100
	nx := 200 ny := 100 ns := 100
//	nx := 400 ny := 200 ns := 100
//	nx := 1200 ny := 800 ns := 100
	if fnameout != '' {
		fout.writeln('P6')
		nbytes = 3 * ny * nx
		bytes = malloc(nbytes)
		fout.writeln('$nx $ny')
		fout.writeln('255')
	} else {
		println('P3')
		println('$nx $ny')
		println('255')
	}
	lookfrom := vec.Vec3{9, 2, 2.6}
	lookat := vec.Vec3{3, 0.8, 1}
	dist_to_focus := (lookfrom - lookat).length()
	aperture := f32(0.)
	cam := new_camera(
		lookfrom,
		lookat,
		vec.Vec3{0, 1., 0},
		30., f32(nx) / f32(ny),
		aperture,
		dist_to_focus)
//	eprintln(cam.str())
	world := new_world()
	for j := ny - 1; j >= 0; j-- {
//		println('j=$j')
		for i := 0; i < nx; i++ {
//			println('i=$i')
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
			if fnameout != '' {
				bytes[(j * nx + i) * 3 + 0] = ir
				bytes[(j * nx + i) * 3 + 1] = ig
				bytes[(j * nx + i) * 3 + 2] = ib
			} else {
				print('$ir $ig $ib  ')
			}
		}
		if fnameout == '' {
			println('')
		}
	}
	if fnameout != '' {
		fout.write_bytes(bytes, nbytes)
		fout.close()
		unsafe {
			free(bytes)
		}
	}
}
