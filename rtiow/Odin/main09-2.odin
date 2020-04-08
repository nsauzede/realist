package main

import "core:fmt"
import "core:math"

import "pcg"

FLT_MAX : f32 = 340282346638528859811704183484516925440.000000;

Vec3 :: distinct [3]f32;

Ray :: struct {
	origin: Vec3,
	direction: Vec3
}

Material :: union {
	Material_Metal,
	Material_Lambertian,
}

Material_Metal :: struct {
	albedo: Vec3,
	fuzz: f32
}

Material_Lambertian :: struct {
	albedo: Vec3,
}

Hittable :: union {
	Sphere,
}

Sphere :: struct {
	center: Vec3,
	radius: f32,
	material: Material
}

Hit_Record :: struct {
	t: f32,
	p: Vec3,
	normal: Vec3,
	mat: Material
}

Camera :: struct {
	lower_left_corner: Vec3,
	horizontal: Vec3,
	vertical: Vec3,
	origin: Vec3
}

random_f :: proc() -> f32 {
	return f32(pcg.rand()) / (f32(pcg.RAND_MAX) + 1.0);
}

random_in_unit_sphere :: proc() -> Vec3 {
	p := Vec3{};
	for {
		r1 := random_f();
		r2 := random_f();
		r3 := random_f();
		p = 2.0 * Vec3{r1, r2, r3} - Vec3{1,1,1};
		if vsqlen(p) < 1.0 {
			break;
		}
	}
	return p;
}

point_at_parameter :: proc(r: Ray, t: f32) -> Vec3 {
	return r.origin + t * r.direction;
}

vlen :: proc(v: Vec3) -> f32 {
	return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

vsqlen :: proc(v: Vec3) -> f32 {
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

unit_vector :: proc(v: Vec3) -> Vec3 {
	return v / vlen(v);
}

vdot :: proc(v1: Vec3, v2: Vec3) -> f32 {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

vreflect :: proc(v: Vec3, n: Vec3) -> Vec3 {
	return v - 2.0 * vdot(v, n) * n;
}

get_ray :: proc(c: Camera, u: f32, v: f32) -> Ray {
	return Ray{c.origin, c.lower_left_corner + u * c.horizontal + v * c.vertical - c.origin};
}

hit_sphere :: proc(s: Sphere, r: Ray, t_min: f32, t_max: f32, rec: ^Hit_Record) -> bool {
	oc := r.origin - s.center;
	a := vdot(r.direction, r.direction);
	b := vdot(oc, r.direction);
	c := vdot(oc, oc) - s.radius * s.radius;
	discriminant := b * b - a * c;
	if discriminant > 0 {
		temp := (-b - math.sqrt(discriminant)) / a;
		if temp < t_max && temp > t_min {
			rec.t = temp;
			rec.p = point_at_parameter(r, rec.t);
			rec.normal = (rec.p - s.center) / s.radius;
			rec.mat = s.material;
			return true;
		}
		temp = (-b + math.sqrt(discriminant)) / a;
		if temp < t_max && temp > t_min {
			rec.t = temp;
			rec.p = point_at_parameter(r, rec.t);
			rec.normal = (rec.p - s.center) / s.radius;
			rec.mat = s.material;
			return true;
		}
	}
	return false;
}

hit :: proc(world: []Hittable, r: Ray, t_min: f32, t_max: f32, rec: ^Hit_Record) -> bool {
	hit_anything := false;
	closest_so_far := t_max;
	for hittable in world {
		switch h in hittable {
		case Sphere:
			if hit_sphere(h, r, t_min, closest_so_far, rec) {
				hit_anything = true;
				closest_so_far = rec.t;
			}
		}
	}
	return hit_anything;
}

scatter :: proc(material: Material, ray_in: Ray, rec: ^Hit_Record, attenuation: ^Vec3, scattered: ^Ray) -> bool {
	switch m in material {
	case Material_Metal:
		reflected := vreflect(unit_vector(ray_in.direction), rec.normal);
		scattered^ = Ray{rec.p, reflected + m.fuzz * random_in_unit_sphere()};
		attenuation^ = m.albedo;
		return vdot(scattered.direction, rec.normal) > 0;
	case Material_Lambertian:
		target := rec.normal + random_in_unit_sphere();
		scattered^ = Ray{rec.p, target};
		attenuation^ = m.albedo;
		return true;
	}
	return false;
}

color :: proc(world: []Hittable, r: Ray, depth: int) -> Vec3 {
	rec := Hit_Record{};
	if hit(world, r, 0.001, FLT_MAX, &rec) {
		scattered := Ray{};
		attenuation := Vec3{};
		if depth < 50 && scatter(rec.mat, r, &rec, &attenuation, &scattered) {
			return attenuation * color(world, scattered, depth + 1);
		} else {
			return Vec3{0, 0, 0};
		}
	} else {
		unit_direction := unit_vector(r.direction);
		t := 0.5 * (unit_direction[1] + 1.0);
		return (1.0 - t) * Vec3{1.0, 1.0, 1.0} + t * Vec3{0.5, 0.7, 1.0};
	}
}

main :: proc() {
	pcg.srand(0);
	nx := 200;
	ny := 100;
	ns := 100;
	fmt.printf("P3\n");
	fmt.printf("%d %d\n", nx, ny);
	fmt.printf("%d\n", 255);
	world := []Hittable{
		Sphere{Vec3{0, 0, -1}, 0.5, Material_Lambertian{Vec3{0.1, 0.2, 0.5}}},
		Sphere{Vec3{0, -100.5, -1}, 100, Material_Lambertian{Vec3{0.8, 0.8, 0.0}}},
		Sphere{Vec3{1, 0, -1}, 0.5, Material_Metal{Vec3{0.8, 0.6, 0.2}, 1.0}},
		Sphere{Vec3{-1, 0, -1}, 0.5, Material_Metal{Vec3{0.8, 0.8, 0.8}, 0.3}},
	};
	cam := Camera {
		Vec3{-2.,-1.,-1.},
		Vec3{4.,0.,0.},
		Vec3{0.,2.,0.},
		Vec3{0.,0.,0.}
	};
	for j := ny - 1; j >= 0; j -= 1 {
		for i := 0; i < nx; i += 1 {
			col := Vec3{0, 0, 0};
			for s := 0; s < ns; s += 1 {
				u := (f32(i) + random_f()) / f32(nx);
				v := (f32(j) + random_f()) / f32(ny);
				r := get_ray(cam, u, v);
				col += color(world, r, 0);
			}
			col /= f32(ns);
			col = Vec3{math.sqrt(col[0]), math.sqrt(col[1]), math.sqrt(col[2])};
			ir := int(255.99 * col[0]);
			ig := int(255.99 * col[1]);
			ib := int(255.99 * col[2]);
			fmt.printf("%d %d %d\n", ir, ig, ib);
		}
	}
}
