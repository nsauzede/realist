#!/bin/env -S v run

module main

fn main() {
	nx := 200
	ny := 100
	println('P3')
	println('$nx $ny')
	println(255)
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			r := f32(i) / f32(nx)
			g := f32(j) / f32(ny)
			b := 0.2
			ir := int(255.99 * r)
			ig := int(255.99 * g)
			ib := int(255.99 * b)
			println('$ir $ig $ib')
		}
	}
}
