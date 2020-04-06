package main

import "core:fmt"
import "core:math"

Vec3 :: [3]f32;

Ray :: struct {
	origin: Vec3,
	direction: Vec3
}

vlen :: proc(v: Vec3) -> f32 {
	return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

unit_vector :: proc(v: Vec3) -> Vec3 {
	return v / vlen(v);
}

vdot :: proc(v1: Vec3, v2: Vec3) -> f32 {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

hit_sphere :: proc(center: Vec3, radius: f32, r: Ray) -> bool {
	oc := r.origin - center;
	a := vdot(r.direction, r.direction);
	b := 2.0 * vdot(oc, r.direction);
	c := vdot(oc, oc) - radius * radius;
	discriminant := b * b - 4 * a * c;
	return discriminant > 0;
}

color :: proc(r: Ray) -> Vec3 {
	if hit_sphere(Vec3{0, 0, -1}, 0.5, r) {
		return Vec3{1, 0, 0};
	}
	unit_direction := unit_vector(r.direction);
	t := 0.5 * (unit_direction[1] + 1.0);
	return (1.0 - t) * Vec3{1.0, 1.0, 1.0} + t * Vec3{0.5, 0.7, 1.0};
}

main :: proc() {
	nx := 200;
	ny := 100;
	fmt.printf("P3\n");
	fmt.printf("%d %d\n", nx, ny);
	fmt.printf("%d\n", 255);
	lower_left_corner := Vec3{-2.0, -1.0, -1.0};
	horizontal := Vec3{4.0, 0.0, 0.0};
	vertical := Vec3{0.0, 2.0, 0.0};
	origin := Vec3{0.0, 0.0, 0.0};
	for j := ny - 1; j >= 0; j -= 1 {
		for i := 0; i < nx; i += 1 {
			u := f32(i) / f32(nx);
			v := f32(j) / f32(ny);
			r := Ray{origin, lower_left_corner + u * horizontal + v * vertical};
			col := color(r);
			ir := int(255.99 * col[0]);
			ig := int(255.99 * col[1]);
			ib := int(255.99 * col[2]);
			fmt.printf("%d %d %d\n", ir, ig, ib);
		}
	}
}
