module main

import vec
import ray
import math
import pcg

struct HitRec {
mut:
	t      f32 // hit time
	p      vec.Vec3 // hit point coords
	normal vec.Vec3 // normal at hit point
	mat &Material=voidptr(0) // hittable at hit point (material, etc..)
}

[inline]
fn random_f() f32 {
	$if dbg ? {
		rfcnt++
	}
	return f32(pcg.pcg_rand()) / (f32(pcg.pcg_rand_max) + 1.0)
}

fn random_in_unit_sphere() vec.Vec3 {
	$if dbg ? {
		riuscnt++
	}
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

fn random_in_unit_disk() vec.Vec3 {
	$if dbg ? {
		riudcnt++
	}
	mut p := vec.Vec3{}
	for {
		r1 := random_f()
		r2 := random_f()
		p = vec.mult(2, vec.Vec3{r1, r2, 0}) - vec.Vec3{1, 1, 0}
		if p.dot(p) < 1.0 {
			break
		}
	}
	return p
}

struct Lambertian {
	albedo    vec.Vec3
}

struct Metal {
	albedo    vec.Vec3
	fuzz      f32 = f32(0.)
}

struct Dielectric {
	ref_idx   f32
}

type Material = Lambertian | Metal | Dielectric

struct Sphere {
	material Material
	center   vec.Vec3
	radius   f32
}
struct NullHittable{}
type Hittable = Sphere | NullHittable

pub fn (s &Sphere) str() string {
	return '{HS:$s.center,$s.radius,${s.material.str()}}'
}

fn (s &Sphere) hit(r ray.Ray, t_min f32, t_max f32, mut rec HitRec) bool {
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
			rec.mat = &s.material
			return true
		}
		temp = (-b + math.sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vec.div(rec.p - s.center, s.radius)
			rec.mat = &s.material
			return true
		}
	}
	return false
}

[inline]
fn (h &Hittable) hit(r ray.Ray, t_min f32, t_max f32, mut rec HitRec) bool {
	match h {
		Sphere {
			return h.hit(r, t_min, t_max, mut rec)
		}
		NullHittable {}
	}
	return false
}

fn (hh []Hittable) hit(r ray.Ray, t_min f32, t_max f32, mut rec HitRec) bool {
	mut hit_anything := false
	mut closest_so_far := t_max
	for h in hh {
		if h.hit(r, t_min, closest_so_far, mut rec) {
			hit_anything = true
			closest_so_far = rec.t
		}
	}
	return hit_anything
}

fn cb_str_lambertian(obj voidptr) string {
	l := &Lambertian(obj)
	return '{ML:$l.albedo}'
}

fn (l &Lambertian) scatter(r_in ray.Ray, rec HitRec, mut attenuation vec.Vec3, mut scattered ray.Ray) bool {
	target := rec.normal + random_in_unit_sphere()
	unsafe {
		*scattered = ray.Ray{rec.p, target}
		*attenuation = l.albedo
	}
	return true
}

pub fn (m &Metal) str() string {
	return '{MM:$m.albedo,$m.fuzz}'
}

fn (m &Metal) scatter(r_in ray.Ray, rec HitRec, mut attenuation vec.Vec3, mut scattered ray.Ray) bool {
	reflected := r_in.direction().unit_vector().reflect(rec.normal)
	unsafe {
		*scattered = ray.Ray{rec.p, reflected + vec.mult(m.fuzz, random_in_unit_sphere())}
		*attenuation = m.albedo
	}
	return scattered.direction().dot(rec.normal) > 0
}

[inline]
fn schlick(cosine f32, ref_idx f32) f32 {
	mut r0 := (1.0 - ref_idx) / (1.0 + ref_idx)
	r0 = r0 * r0
	return r0 + (1.0 - r0) * math.powf(1.0 - cosine, 5)
}

fn cb_str_dielectric(obj voidptr) string {
	d := &Dielectric(obj)
	return '{MD:$d.ref_idx}'
}

fn (d &Dielectric) scatter(r_in ray.Ray, rec HitRec, mut attenuation vec.Vec3, mut scattered ray.Ray) bool {
	mut outward_normal := vec.Vec3{}
	reflected := r_in.direction().reflect(rec.normal)
	mut ni_over_nt := f32(0)
	unsafe {
		*attenuation = vec.Vec3{1, 1, 1}
	}
	mut refracted := vec.Vec3{}
	mut reflect_prob := f32(0)
	mut cosine := f32(0)
	dot := r_in.direction().dot(rec.normal)
	len := r_in.direction().length()
	if dot > 0 {
		outward_normal = vec.mult(-1, rec.normal)
		ni_over_nt = d.ref_idx
		cosine = d.ref_idx * dot / len
	} else {
		outward_normal = rec.normal
		ni_over_nt = 1.0 / d.ref_idx
		cosine = -dot / len
	}
	// dln := vec.Vec3{dot, len, ni_over_nt}
	// println('dln=$dln')
	// println('outnorm=$outward_normal')
	// println('rindir=$r_in.direction()')
	if r_in.direction().refract(outward_normal, ni_over_nt, mut refracted) {
		// println('SCHLICK')
		reflect_prob = schlick(cosine, d.ref_idx)
	} else {
		// println('NOSCHLICK')
		reflect_prob = 1
	}
	if random_f() < reflect_prob {
		// println('REFL')
		unsafe {
			*scattered = ray.Ray{rec.p, reflected}
		}
	} else {
		// println('REFR')
		unsafe {
			*scattered = ray.Ray{rec.p, refracted}
		}
	}
	return true
}

[inline]
fn (m &Material) scatter(r_in ray.Ray, rec HitRec, mut attenuation vec.Vec3, mut scattered ray.Ray) bool {
	match m {
		Lambertian {
			return m.scatter(r_in, rec, mut attenuation, mut scattered)
		}
		Metal {
			return m.scatter(r_in, rec, mut attenuation, mut scattered)
		}
		Dielectric {
			return m.scatter(r_in, rec, mut attenuation, mut scattered)
		}
	}
	return false
}

fn (world []Hittable) color(r ray.Ray, depth int) vec.Vec3 {
	$if dbg ? {
		// println(r)
		// v := vec.Vec3{math.max_f32, 0, 0}
		// println('$v')
	}
	mut rec := HitRec{}
	// remove acne by starting at 0.001
	if world.hit(r, 0.001, math.max_f32, mut rec) {
		$if dbg ? {
			println('HIT')
		}
		mut scattered := ray.Ray{}
		mut attenuation := vec.Vec3{}
		if depth < 50 &&
			rec.mat.scatter(r, rec, mut &attenuation, mut &scattered) {
			$if dbg ? {
				println('ATT')
				tv := vec.Vec3{rec.t, 0, 0}
				println('tv=$tv')
				println('p=$rec.p')
				println('nor=$rec.normal')
				println('h=${h.generic.strcb(h)}')
				println('sca=$scattered')
			}
			return attenuation * world.color(scattered, depth + 1)
		} else {
			$if dbg ? {
				println('NOT ATT')
			}
			return vec.Vec3{0, 0, 0}
		}
	} else {
		unit_direction := r.direction().unit_vector()
		$if dbg ? {
			// println('NOT HIT')
			println('NOT HIT dir=${r.direction()}ud=$unit_direction')
		}
		t := .5 * (unit_direction.y + 1.0)
		// return vec.mult(1.0 - t, vec.Vec3{1, 1, 1}) + vec.mult(t, vec.Vec3{.5, .7, 1})
		col0 := vec.mult(1.0 - t, vec.Vec3{1, 1, 1})
		col1 := vec.mult(t, vec.Vec3{.5, .7, 1})
		return col0 + col1
	}
}

struct Camera {
mut:
	origin            vec.Vec3
	lower_left_corner vec.Vec3
	horizontal        vec.Vec3
	vertical          vec.Vec3
	u                 vec.Vec3
	v                 vec.Vec3
	w                 vec.Vec3
	lens_radius       f32
}

pub fn (c &Camera) str() string {
	return '{origin = $c.origin, lower_left_corner = $c.lower_left_corner,
	horizontal = $c.horizontal, vertical = $c.vertical,
	u = $c.u, v = $c.v, w = $c.w,
	lens_radius = $c.lens_radius'
}

fn (mut cam Camera) make(lookfrom vec.Vec3, lookat vec.Vec3, vup vec.Vec3, vfov f32, aspect f32, aperture f32, focus_dist f32) {
	// println(lookfrom)
	theta := vfov * math.pi / 180.0
	half_height := math.tanf(theta / 2.0)
	half_width := aspect * half_height
	w := (lookfrom - lookat).unit_vector()
	u := vup.cross(w).unit_vector()
	v := w.cross(u)
	cam.lens_radius = aperture / 2
	cam.origin = lookfrom
	cam.lower_left_corner = lookfrom - vec.mult(half_width * focus_dist, u) - vec.mult(half_height *
		focus_dist, v) - vec.mult(focus_dist, w)
	cam.horizontal = vec.mult(2.0 * half_width * focus_dist, u)
	cam.vertical = vec.mult(2.0 * half_height * focus_dist, v)
	cam.u = u
	cam.v = v
	cam.w = w
}

[inline]
fn (c &Camera) get_ray(s f32, t f32) ray.Ray {
	$if dbg ? {
		// println('s=$s t=$t')
	}
	rd := vec.mult(c.lens_radius, random_in_unit_disk())
	offset := vec.mult(rd.x, c.u) + vec.mult(rd.y, c.v)
	return ray.Ray{c.origin + offset, c.lower_left_corner + vec.mult(s, c.horizontal) + vec.mult(t, c.vertical) -
		c.origin - offset}
}

pub fn (h &Hittable) str() string {
	match h {
		Sphere {
			return h.str()
		}
		NullHittable{}
	}
	return ''
}

fn main() {
	pcg.pcg_srand(0)
	nx := 200
	ny := 100
	ns := 100
	println('P3')
	println('$nx $ny')
	println(255)
	lookfrom := vec.Vec3{3, 3, 2}
	lookat := vec.Vec3{0, 0, -1}
	dist_to_focus := (lookfrom - lookat).length()
	aperture := f32(2)
	mut cam := Camera{}
	cam.make(lookfrom, lookat, vec.Vec3{0, 1, 0}, 20, f32(nx) / f32(ny), aperture, dist_to_focus)
	world := [
		Hittable(
			Sphere {
				center: vec.Vec3{0, 0, -1}
				radius: 0.5
				material: Lambertian {
						albedo: vec.Vec3{0.1, 0.2, 0.5}
				}
			}
		),
		Hittable(
			Sphere {
				center: vec.Vec3{0, -100.5, -1}
				radius: 100
				material: Lambertian {
						albedo: vec.Vec3{0.8, 0.8, 0.0}
				}
			}
		),
		Hittable(
			Sphere {
				center: vec.Vec3{1, 0, -1}
				radius: 0.5
				material: Metal {
						albedo: vec.Vec3{0.8, 0.6, 0.2}
						fuzz: 0.3
				}
			}
		),
		Hittable(
			Sphere {
				center: vec.Vec3{-1, 0, -1}
				radius: 0.5
				material: Dielectric {
						ref_idx: 1.5
				}
			}
		),
		Hittable(
			Sphere {
				center: vec.Vec3{-1, 0, -1}
				radius: -0.45
				material: Dielectric {
						ref_idx: 1.5
				}
			}
		),
	]
	$if dbg ? {
		// println(cam)
		// println(world)
	}
	for j := ny - 1; j >= 0; j-- {
		// if j==98 {break}
		for i := 0; i < nx; i++ {
			mut col := vec.Vec3{0, 0, 0}
			for s := 0; s < ns; s++ {
				$if dbg ? {
					println('rfcnt=$rfcnt riuscnt=$riuscnt riudcnt=$riudcnt')
				}
				u := (f32(i) + random_f()) / f32(nx)
				v := (f32(j) + random_f()) / f32(ny)
				$if dbg ? {
					// println('u=$u v=$v rfcnt=$rfcnt riuscnt=$riuscnt')
					// println('u=$u v=$v')
					// uv := vec.Vec3{u, v, 0}
					// println('uv=$uv')
				}
				r := cam.get_ray(u, v)
				$if dbg ? {
					// println('j=$j i=$i s=$s r=$r')
					// println('r=$r')
				}
				// col = col + world.color(r, 0)
				col0 := world.color(r, 0)
				$if dbg ? {
					// println('col0=$col0')
				}
				col = col + col0
				// println('col=$col')
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
