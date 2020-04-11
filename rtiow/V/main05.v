#!/bin/env -S v run

module main

import vec
import ray

fn hit_sphere(center vec.Vec3, radius f32, r ray.Ray) bool {
	oc := r.origin() - center
//	println('oc=$oc')
	a := r.direction().dot(r.direction())
	b := 2.0 * oc.dot(r.direction())
	c := oc.dot(oc) - radius * radius
	discriminant := b * b - 4.0 * a * c
//	println('a=$a b=$b c=$c d=$discriminant')
	return discriminant > 0
}

fn color(r ray.Ray) vec.Vec3 {
	// next 2 lines to workaround buggy AST v2 compiler
	x := vec.Vec3{0, 0, -1}
	if hit_sphere(x, 0.5, r) {
//	if hit_sphere(vec.Vec3{0, 0, -1}, 0.5, r) {
		return vec.Vec3{1, 0, 0}
	}
	unit_direction := r.direction().unit_vector()
	t := .5 * (unit_direction.y + 1.0)
	return vec.mult(1.0 - t, vec.Vec3{1, 1, 1}) + vec.mult(t, vec.Vec3{.5, .7, 1})
}

fn main() {
	nx := 200
	ny := 100
	println('P3')
	println('$nx $ny')
	println(255)
	lower_left_corner := vec.Vec3 {-2, -1, -1}
	horizontal := vec.Vec3 {4, 0, 0}
	vertical := vec.Vec3 {0, 2, 0}
	origin := vec.Vec3 {0, 0, 0}
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			u := f32(i) / f32(nx)
			v := f32(j) / f32(ny)
			r := ray.Ray {
				origin,
				lower_left_corner
					+ vec.mult(u, horizontal)
					+ vec.mult(v, vertical)
			}
			col := color(r)
			ir := int(f32(255.99) * col.x)
			ig := int(f32(255.99) * col.y)
			ib := int(f32(255.99) * col.z)
			println('$ir $ig $ib')
		}
	}
}
