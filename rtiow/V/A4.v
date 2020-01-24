#!/bin/env -S v run

module main

import vec
import ray

fn color(r ray.Ray) vec.Vec3 {
	unit_direction := r.direction().unit_vector()
	t := .5 * (unit_direction.y + 1.)
	return vec.mult(1. - t, vec.Vec3{1., 1., 1.}) + vec.mult(t, vec.Vec3{.5, .7, 1})
}

fn main() {
	nx := 200
	ny := 100
	println('P3')
	println('$nx $ny')
	println(255)
	lower_left_corner := vec.Vec3 {-2., -1., -1.}
	horizontal := vec.Vec3 {4., 0., 0.}
	vertical := vec.Vec3 {0., 2., 0.}
	origin := vec.Vec3 {0., 0., 0.}
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
			ir := int(255.99 * col.x)
			ig := int(255.99 * col.y)
			ib := int(255.99 * col.z)
			println('$ir $ig $ib')
		}
	}
}
