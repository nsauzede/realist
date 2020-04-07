use std::ops::Div;
use std::ops::Mul;

#[derive(Debug)]
#[derive(Copy, Clone)]
struct Vec3([f32; 3]);

#[derive(Debug)]
#[derive(Copy, Clone)]
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

impl std::ops::Rem for Vec3 {
	type Output = f32;

	fn rem(self, rhs: Vec3) -> f32 {
		let Self([x1, y1, z1]) = self;
		let Self([x2, y2, z2]) = rhs;

		x1 * x2 + y1 * y2 + z1 * z2
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

impl std::ops::Sub for Vec3 {
	type Output = Vec3;

	fn sub(self, rhs: Vec3) -> Self::Output {
		let Self([x1, y1, z1]) = self;
		let Self([x2, y2, z2]) = rhs;

		Self([x1 - x2, y1 - y2, z1 - z2])
	}
}

impl Div<f32> for Vec3 {
	type Output = Self;

	fn div(self, rhs: f32) -> Self::Output {
		let Self([x1, y1, z1]) = self;
		Self([x1 / rhs, y1 / rhs, z1 / rhs])
	}
}
/*
impl Mul<f32> for Vec3 {
	type Output = Self;

	fn mul(self, rhs: f32) -> Self::Output {
		let Self([x1, y1, z1]) = self;
		Self([x1 * rhs, y1 * rhs, z1 * rhs])
	}
}
*/
impl Mul<Vec3> for f32 {
	type Output = Vec3;

	fn mul(self, rhs: Vec3) -> Vec3 {
//		rhs * self
		Vec3([self * rhs.0[0], self * rhs.0[1], self * rhs.0[2]])
	}
}

fn unit_vector(v: Vec3) -> Vec3 {
	v / v.length()
}

impl Ray {
	fn point_at_parameter(self, t: f32) -> Vec3 {
		self.origin + t * self.direction
	}
}

fn hit_sphere(center: Vec3, radius: f32, r: Ray) -> f32 {
	let oc = r.origin - center;
//	println!("oc={:?}", oc);
	let a = r.direction % r.direction;
	let b = 2. * (oc % r.direction);
	let c = (oc % oc) - radius * radius;
	let discriminant = b * b - 4. * a * c;
//	println!("a={} b={} c={} d={}", a, b, c, discriminant);
	if discriminant < 0. {
		-1.
	} else {
		(-b - f32::sqrt(discriminant)) / (2. * a)
	}
}

fn color(r: Ray) -> Vec3 {
	let t = hit_sphere(Vec3([0., 0., -1.]), 0.5, r);
	if t > 0. {
//		println!("t={}", t);
		let n = unit_vector(r.point_at_parameter(t) - Vec3([0., 0., -1.]));
		0.5 * (n + Vec3([1., 1., 1.]))
	} else {
		let unit_direction = unit_vector(r.direction);
		let t = 0.5 * (unit_direction.0[1] + 1.);
		(1. - t) * Vec3([1., 1., 1.]) + t * Vec3([0.5, 0.7, 1.])
	}
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
			let r = Ray {
				origin: origin,
				direction: lower_left_corner + u * horizontal + v * vertical
			};
			let col = color(r);
			let _ir = (255.99*col.0[0]) as i32;
			let _ig = (255.99*col.0[1]) as i32;
			let _ib = (255.99*col.0[2]) as i32;
			println!("{} {} {}", _ir, _ig, _ib);
		}
	}
}
