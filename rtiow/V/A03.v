#!/bin/env -S v run

module main

import vec

fn main() {
	nx := 200
	ny := 100
	println('P3')
	println('$nx $ny')
	println(255)
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			col := vec.Vec3{f32(i) / f32(nx), f32(j) / f32(ny), 0.2}
			ir := int(f32(255.99) * col.x)
			ig := int(f32(255.99) * col.y)
			ib := int(f32(255.99) * col.z)
			println('$ir $ig $ib')
		}
	}
}
