package main

import "core:fmt"
import "core:math"

Vec3 :: [3]f32;

Ray :: struct {
	origin: Vec3,
	direction: Vec3
};

vlen :: proc(v: Vec3) -> f32 {
	return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

vdiv :: proc(v: Vec3, f: f32) -> Vec3 {
	return Vec3{v[0] / f, v[1] / f, v[2] / f};
}

unit_vector :: proc(v: Vec3) -> Vec3 {
	return vdiv(v, vlen(v));
}

point_at_parameter :: proc(r: Ray, t: f32) -> Vec3 {
	return vadd(r.origin, vmul(t, r.direction));
}

vadd :: proc(v1: Vec3, v2: Vec3) -> Vec3 {
	return Vec3{v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]};
}

vsub :: proc(v1: Vec3, v2: Vec3) -> Vec3 {
	return Vec3{v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]};
}

vmul :: proc(f: f32, v: Vec3) -> Vec3 {
	return Vec3{f * v[0], f * v[1], f * v[2]};
}

vdot :: proc(v1: Vec3, v2: Vec3) -> f32 {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

hit_sphere :: proc(center: Vec3, radius: f32, r: Ray) -> f32 {
	oc := vsub(r.origin, center);
//	fmt.println("oc=", oc);
	a := vdot(r.direction, r.direction);
	b := 2.0 * vdot(oc, r.direction);
	c := vdot(oc, oc) - radius * radius;
//	fmt.println("abc=", Vec3{a, b, c});
	discriminant := b * b - 4 * a * c;
	if discriminant < 0 {
		return -1.0;
	} else {
		return (-b - math.sqrt(discriminant)) / (2.0 * a);
	}
}

color :: proc(r: Ray) -> Vec3 {
	t := hit_sphere(Vec3{0, 0, -1}, 0.5, r);
	if t > 0.0 {
		N := unit_vector(vsub(point_at_parameter(r, t), Vec3{0, 0, -1}));
		return vmul(0.5, Vec3{N[0] + 1, N[1] + 1, N[2] + 1});
	}
	unit_direction := unit_vector(r.direction);
	t = 0.5 * (unit_direction[1] + 1.0);
	return vadd(vmul(1.0 - t, Vec3{1.0, 1.0, 1.0}), vmul(t, Vec3{0.5, 0.7, 1.0}));
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
			r := Ray{origin, vadd(lower_left_corner, vadd(vmul(u, horizontal), vmul(v, vertical)))};
			col := color(r);
			ir := int(255.99 * col[0]);
			ig := int(255.99 * col[1]);
			ib := int(255.99 * col[2]);
			fmt.printf("%d %d %d\n", ir, ig, ib);
		}
	}
}
