package main

import "core:fmt"

Vec3 :: [3]f32;

Ray :: struct {
	origin: Vec3,
	direction: Vec3
};

point_at_parameter :: proc(r: Ray, t: f32) {
}

main :: proc() {
	nx := 200;
	ny := 100;
	fmt.printf("P3\n");
	fmt.printf("%d %d\n", nx, ny);
	fmt.printf("%d\n", 255);
	for j := ny - 1; j >= 0; j -= 1 {
		for i := 0; i < nx; i += 1 {
			col := Vec3{f32(i) / f32(nx), f32(j) / f32(ny), 0.2};
			ir := int(255.99 * col[0]);
			ig := int(255.99 * col[1]);
			ib := int(255.99 * col[2]);
			fmt.printf("%d %d %d\n", ir, ig, ib);
		}
	}
}
