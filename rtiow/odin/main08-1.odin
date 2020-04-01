package main

import "core:fmt"
import "core:math"

foreign import libc "system:c"

import "core:c"

@(default_calling_convention="c")
foreign libc {
	rand :: proc() -> c.int ---
	srand :: proc(seed: c.uint) -> c.int ---
}

Vec3 :: [3]f32;

Ray :: struct {
	origin: Vec3,
	direction: Vec3
};

HSphere :: struct {
	center: Vec3,
	radius: f32
};

HitRec :: struct {
	t: f32,
	p: Vec3,
	normal: Vec3
};

Camera :: struct {
	lower_left_corner: Vec3,
	horizontal: Vec3,
	vertical: Vec3,
	origin: Vec3
};

get_ray :: proc(c: Camera, u: f32, v: f32) -> Ray {
	return Ray{c.origin, c.lower_left_corner + u * c.horizontal + v * c.vertical - c.origin};
}

random_double :: proc() -> f32 {
	RAND_MAX : u32 = 2147483647;
	return f32(rand()) / (f32(RAND_MAX) + 1.0);
}

vlen :: proc(v: Vec3) -> f32 {
	return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

squared_length :: proc(v: Vec3) -> f32 {
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

unit_vector :: proc(v: Vec3) -> Vec3 {
	return v / vlen(v);
}

point_at_parameter :: proc(r: Ray, t: f32) -> Vec3 {
	return r.origin + t * r.direction;
}

vdot :: proc(v1: Vec3, v2: Vec3) -> f32 {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

hit_sphere :: proc(s: HSphere, r: Ray, t_min: f32, t_max: f32, rec: ^HitRec) -> bool {
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
			return true;
		}
		temp = (-b + math.sqrt(discriminant)) / a;
		if temp < t_max && temp > t_min {
			rec.t = temp;
			rec.p = point_at_parameter(r, rec.t);
			rec.normal = (rec.p - s.center) / s.radius;
			return true;
		}
	}
	return false;
}

hit :: proc(hh: []HSphere, r: Ray, t_min: f32, t_max: f32, rec: ^HitRec) -> bool {
	temp_rec := HitRec{};
	hit_anything := false;
	closest_so_far := t_max;
	for h in hh {
		if hit_sphere(h, r, t_min, closest_so_far, &temp_rec) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec^ = temp_rec;
		}
	}
	return hit_anything;
}

random_in_unit_sphere :: proc() -> Vec3 {
	p := Vec3{};
	for {
		r1 := random_double();
		r2 := random_double();
		r3 := random_double();
		p = 2.0 * Vec3{r1, r2, r3} - Vec3{1,1,1};
		if squared_length(p) < 1.0 {
			break;
		}
	}
	return p;
}

color :: proc(r: Ray, world: []HSphere) -> Vec3 {
	rec := HitRec{};
	if hit(world, r, 0.001, 99999., &rec) {
		target := rec.normal + random_in_unit_sphere();
		return 0.5 * color(Ray{rec.p, target}, world);
	}
	unit_direction := unit_vector(r.direction);
	t := 0.5 * (unit_direction[1] + 1.0);
	return (1.0 - t) * Vec3{1.0, 1.0, 1.0} + t * Vec3{0.5, 0.7, 1.0};
}

main :: proc() {
	srand(0);
	nx := 200;
	ny := 100;
	ns := 100;
	fmt.printf("P3\n");
	fmt.printf("%d %d\n", nx, ny);
	fmt.printf("%d\n", 255);
	world := []HSphere{
		HSphere{Vec3{0.0, 0.0, -1.0}, 0.5},
		HSphere{Vec3{0.0, -100.5, -1.0}, 100}
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
				u := (f32(i) + random_double()) / f32(nx);
				v := (f32(j) + random_double()) / f32(ny);
				r := get_ray(cam, u, v);
				col += color(r, world);
			}
			col /= f32(ns);
			ir := int(255.99 * col[0]);
			ig := int(255.99 * col[1]);
			ib := int(255.99 * col[2]);
			fmt.printf("%d %d %d\n", ir, ig, ib);
		}
	}
}
