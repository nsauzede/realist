use std::ops::Div;
use std::ops::Mul;

#[derive(Debug)]
#[derive(Copy, Clone)]
struct Vec3([f32; 3]);

#[derive(Debug)]
struct Ray {
	origin: Vec3,
	direction: Vec3
}

impl Vec3 {
	fn squared_length(self) -> f32 {
		let Self([x1, y1, z1]) = self;
		x1 * x1 + y1 * y1 + z1 * z1
	}
	fn length(self) -> f32 {
		f32::sqrt(self.squared_length())
	}
}

impl std::ops::Div for Vec3 {
    type Output = Vec3;

    fn div(self, rhs: Vec3) -> Self::Output {
        let Self([x1, y1, z1]) = self;
        let Self([x2, y2, z2]) = rhs;

        Self([x1 / x2, y1 / y2, z1 / z2])
    }
}

impl std::ops::Add for Vec3 {
    type Output = Vec3;

    fn add(self, rhs: Vec3) -> Self::Output {
        let Self([x1, y1, z1]) = self;
        let Self([x2, y2, z2]) = rhs;

        Self([x1 + x2, y1 + y2, z1 + z2])
    }
}

impl Div<f32> for Vec3 {
	type Output = Self;

	fn div(self, rhs: f32) -> Self::Output {
		let Self([x1, y1, z1]) = self;
		Self([x1 / rhs, y1 / rhs, z1 / rhs])
	}
}

impl Mul<f32> for Vec3 {
	type Output = Self;

	fn mul(self, rhs: f32) -> Self::Output {
		let Self([x1, y1, z1]) = self;
		Self([x1 * rhs, y1 * rhs, z1 * rhs])
	}
}

//let v = Vec3([0., 0., 0.]);
//v.0[1]

fn unit_vector(v: Vec3) -> Vec3 {
	v / v.length()
}

fn color(r: Ray) -> Vec3 {
//	println!("r={:?}", r);
	let unit_direction = unit_vector(r.direction);
//	println!("ud={:?}", unit_direction);
	let t = 0.5 * (unit_direction.0[1] + 1.);
//	println!("t={}", t);
	Vec3([1., 1., 1.]) * (1. - t) + Vec3([0.5, 0.7, 1.]) * t
}

fn main() {
	let nx = 200;
	let ny = 100;
	println!("P3");
	println!("{} {}", nx, ny);
	println!("255");
	let lower_left_corner = Vec3([-2.0, -1.0, -1.0]);
	let horizontal = Vec3([4., 0., 0.]);
	let vertical = Vec3([0., 2., 0.]);
	let origin = Vec3([0., 0., 0.]);
	for j in (0..ny).rev() {
		for i in 0..nx {
			let u = i as f32 / nx as f32;
			let v = j as f32 / ny as f32;
//			println!("u={} v={}", u, v);
			let r = Ray {
				origin: origin,
				direction: lower_left_corner + horizontal * u + vertical * v
			};
			let col = color(r);
//			println!("col={:?}", col);
			let ir = (255.99*col.0[0]) as i32;
			let ig = (255.99*col.0[1]) as i32;
			let ib = (255.99*col.0[2]) as i32;
			println!("{} {} {}", ir, ig, ib);
		}
	}
}
