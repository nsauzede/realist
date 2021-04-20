module main

import vec
import ray
import math
import pcg

// __global rfcnt int
// __global riuscnt int
// __global riudcnt int
struct HitRec {
mut:
	t      f32 // hit time
	p      vec.Vec3 // hit point coords
	normal vec.Vec3 // normal at hit point
	ph     voidptr // hittable at hit point (material, etc..)
}

fn random_f() f32 {
	// rfcnt++
	return f32(pcg.pcg_rand()) / (f32(pcg.pcg_rand_max) + 1.0)
}

fn random_in_unit_sphere() vec.Vec3 {
	// riuscnt++
	mut p := vec.Vec3{}
	for {
		r1 := random_f()
		r2 := random_f()
		r3 := random_f()
		p = vec.mult(2, vec.Vec3{r1, r2, r3}) - vec.Vec3{1, 1, 1}
		if p.squared_length() < 1.0 {
			break
		}
	}
	return p
}

enum HType {
	sphere
}

type ScatterCallback = fn (obj voidptr, r_in ray.Ray, rec HitRec, mut attenuation vec.Vec3, mut scattered ray.Ray) bool

struct MGeneric {
	scattercb ScatterCallback
}

struct MLambertian {
	scattercb ScatterCallback = main.ScatterCallback(cb_scatter_lambertian)
	albedo    vec.Vec3
}

struct MMetal {
	scattercb ScatterCallback = main.ScatterCallback(cb_scatter_metal)
	albedo    vec.Vec3
	fuzz      f32 = f32(0.)
}

union Material {
	generic    MGeneric
	lambertian MLambertian
	metal      MMetal
}

type StringCallback = fn (obj voidptr) string

type HitCallback = fn (obj voidptr, r ray.Ray, t_min f32, closest f32, mut rec HitRec) bool

struct HGeneric {
	strcb    StringCallback
	hitcb    HitCallback
	material Material
}

struct HSphere {
	strcb    StringCallback = main.StringCallback(cb_sphere_str)
	hitcb    HitCallback = main.HitCallback(cb_sphere_hit)
	material Material
	center   vec.Vec3
	radius   f32
}

union Hittable {
	generic HGeneric
	sphere  HSphere
}

fn cb_sphere_str(obj voidptr) string {
	s := &HSphere(obj)
	return '{S:$s.center,$s.radius}'
}

fn cb_sphere_hit(obj voidptr, r ray.Ray, t_min f32, t_max f32, mut rec HitRec) bool {
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

fn (hh []Hittable) hit(r ray.Ray, t_min f32, t_max f32, mut rec HitRec) bool {
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

fn cb_scatter_lambertian(obj voidptr, r_in ray.Ray, rec HitRec, mut attenuation vec.Vec3, mut scattered ray.Ray) bool {
	l := &MLambertian(obj)
	target := rec.normal + random_in_unit_sphere()
	unsafe {
		*scattered = ray.Ray{rec.p, target}
		*attenuation = l.albedo
	}
	return true
}

fn cb_scatter_metal(obj voidptr, r_in ray.Ray, rec HitRec, mut attenuation vec.Vec3, mut scattered ray.Ray) bool {
	m := &MMetal(obj)
	reflected := r_in.direction().unit_vector().reflect(rec.normal)
	unsafe {
		*scattered = ray.Ray{rec.p, reflected + vec.mult(m.fuzz, random_in_unit_sphere())}
		*attenuation = m.albedo
	}
	return scattered.direction().dot(rec.normal) > 0
}

fn (world []Hittable) color(r ray.Ray, depth int) vec.Vec3 {
	mut rec := HitRec{
		ph: 0
	}
	// remove acne by starting at 0.001
	if world.hit(r, 0.001, math.max_f32, mut rec) {
		// println('HIT')
		mut scattered := ray.Ray{}
		mut attenuation := vec.Vec3{}
		h := &Hittable(rec.ph)
		if depth < 50 &&
			h.generic.material.generic.scattercb(&h.generic.material.generic, r, rec, mut &attenuation, mut &scattered) {
			// println('ATT')
			return attenuation * world.color(scattered, depth + 1)
		} else {
			// println('NOT ATT')
			return vec.Vec3{0, 0, 0}
		}
	} else {
		// println('NOT HIT')
		unit_direction := r.direction().unit_vector()
		t := .5 * (unit_direction.y + 1.0)
		return vec.mult(1.0 - t, vec.Vec3{1, 1, 1}) + vec.mult(t, vec.Vec3{.5, .7, 1})
	}
}

struct Camera {
	lower_left_corner vec.Vec3
	horizontal        vec.Vec3
	vertical          vec.Vec3
	origin            vec.Vec3
}

fn (c Camera) get_ray(u f32, v f32) ray.Ray {
	return ray.Ray{c.origin, c.lower_left_corner + vec.mult(u, c.horizontal) + vec.mult(v, c.vertical) -
		c.origin}
}

fn (h Hittable) str() string {
	return h.generic.strcb(&h)
}

fn main() {
	pcg.pcg_srand(0)
	nx := 200
	ny := 100
	ns := 100
	println('P3')
	println('$nx $ny')
	println(255)
	cam := Camera{
		lower_left_corner: vec.Vec3{-2, -1, -1}
		horizontal: vec.Vec3{4, 0, 0}
		vertical: vec.Vec3{0, 2, 0}
		origin: vec.Vec3{0, 0, 0}
	}
	// println(cam)
	world := [
		Hittable{
			sphere: {
				center: vec.Vec3{0, 0, -1}
				radius: 0.5
				material: {
					lambertian: {
						albedo: vec.Vec3{0.1, 0.2, 0.5}
					}
				}
			}
		},
		Hittable{
			sphere: {
				center: vec.Vec3{0, -100.5, -1}
				radius: 100
				material: {
					lambertian: {
						albedo: vec.Vec3{0.8, 0.8, 0.0}
					}
				}
			}
		},
		Hittable{
			sphere: {
				center: vec.Vec3{1, 0, -1}
				radius: 0.5
				material: {
					metal: {
						albedo: vec.Vec3{0.8, 0.6, 0.2}
						fuzz: 1
					}
				}
			}
		},
		Hittable{
			sphere: {
				center: vec.Vec3{-1, 0, -1}
				radius: 0.5
				material: {
					metal: {
						albedo: vec.Vec3{0.8, 0.8, 0.8}
						fuzz: 0.3
					}
				}
			}
		},
	]
	// println(world)
	for j := ny - 1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			mut col := vec.Vec3{0, 0, 0}
			for s := 0; s < ns; s++ {
				// println('rfcnt=$rfcnt riuscnt=$riuscnt riudcnt=$riudcnt')
				u := (f32(i) + random_f()) / f32(nx)
				v := (f32(j) + random_f()) / f32(ny)
				r := cam.get_ray(u, v)
				// println('j=$j i=$i r=$r')
				col = col + world.color(r, 0)
			}
			col = vec.div(col, ns)
			// Gamma 2 correction (square root)
			col = vec.Vec3{math.sqrtf(col.x), math.sqrtf(col.y), math.sqrtf(col.z)}
			ir := int(f32(255.99) * col.x)
			ig := int(f32(255.99) * col.y)
			ib := int(f32(255.99) * col.z)
			println('$ir $ig $ib')
		}
	}
}
