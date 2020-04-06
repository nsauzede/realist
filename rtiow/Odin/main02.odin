package main

import "core:fmt"

main :: proc() {
	nx := 200;
	ny := 100;
	fmt.printf("P3\n");
	fmt.printf("%d %d\n", nx, ny);
	fmt.printf("%d\n", 255);
	for j := ny - 1; j >= 0; j -= 1 {
		for i := 0; i < nx; i += 1 {
			r := f32(i) / f32(nx);
			g := f32(j) / f32(ny);
			b := 0.2;
			ir := int(255.99 * r);
			ig := int(255.99 * g);
			ib := int(255.99 * b);
			fmt.printf("%d %d %d\n", ir, ig, ib);
		}
	}
}
