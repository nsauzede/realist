mod pcg;

use std::ops::Div;
use std::ops::Mul;
use std::fmt;

#[derive(Debug)]
#[derive(Copy, Clone)]
struct Vec3([f32; 3]);

#[derive(Debug)]
#[derive(Copy, Clone)]
struct Ray {
	origin: Vec3,
	direction: Vec3
}

#[derive(Debug)]
pub struct HSphere {
	center: Vec3,
	radius: f32
}

fn random_f() -> f32 {
	pcg::rand() as f32 / (pcg::RAND_MAX as f32 + 1.0)
}

#[derive(Debug)]
#[derive(Copy, Clone)]
struct Camera {
	origin: Vec3,
	lower_left_corner: Vec3,
	horizontal: Vec3,
	vertical: Vec3
}

impl Camera {
	fn get_ray(self, u: f32, v: f32) -> Ray {
		Ray{
			origin: self.origin,
			direction: self.lower_left_corner + u * self.horizontal + v * self.vertical - self.origin
		}
	}
}

impl fmt::Display for HSphere {
	fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
		write!(f, "{{HS:{} ,{:.6}}}",
			self.center,
			self.radius)
	}
}

pub fn _wprint(world: &[HSphere]) {
	println!("[");
	for h in world {
		print!("{}", h);
		println!(",");
	}
	println!("]");
}

impl fmt::Display for Camera {
	fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
		write!(f, "{{\n\tlower_left_corner: {} \n\thorizontal: {} \n\tvertical: {} \n\torigin: {} \n}}",
			self.lower_left_corner,
			self.horizontal,
			self.vertical,
			self.origin)
	}
}

impl fmt::Display for Ray {
	fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
		write!(f, "{{{}, {}}} ",
			self.origin,
			self.direction)
	}
}

impl fmt::Display for Vec3 {
	fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
unsafe {
		union MyUnion {
			f: [f32; 3],
			i: [u32; 3]
		}
		let u = MyUnion{f:self.0};
		write!(f, "{{{:.6}, {:.6}, {:.6};{:x}, {:x}, {:x}}}",
			self.0[0], self.0[1], self.0[2],
			u.i[0], u.i[1], u.i[2])
}
	}
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

impl Mul<Vec3> for f32 {
	type Output = Vec3;

	fn mul(self, rhs: Vec3) -> Vec3 {
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

#[derive(Copy, Clone)]
struct HitRec {
	t: f32,
	p: Vec3,
	normal: Vec3
}

fn hit_sphere(sphere: &HSphere, r: Ray, t_min: f32, t_max: f32, rec: &mut HitRec) -> bool {
	let center = sphere.center;
	let radius = sphere.radius;
	let oc = r.origin - center;
//	println!("oc={:?}", oc);
	let a = r.direction % r.direction;
	let b = oc % r.direction;
	let c = (oc % oc) - radius * radius;
	let discriminant = b * b - a * c;
//	println!("a={} b={} c={} d={}", a, b, c, discriminant);
	if discriminant > 0. {
		let mut temp = (-b - f32::sqrt(discriminant)) / a;
		if temp < t_max && temp > t_min {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			return true;
		}
		temp = (-b + f32::sqrt(discriminant)) / a;
		if temp < t_max && temp > t_min {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			return true;
		}
	}
	false
}

fn hit(hh: &[HSphere], r: Ray, t_min: f32, t_max: f32, rec: &mut HitRec) -> bool {
	let mut temp_rec = HitRec{t: 0., p: Vec3([0., 0., 0.]), normal: Vec3([0., 0., 0.])};
	let mut hit_anything = false;
	let mut closest_so_far = t_max;
	for h in hh {
		if hit_sphere(&h, r, t_min, closest_so_far, &mut temp_rec) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
//			println!("t={}", temp)
		}
	}
	hit_anything
}



fn random_in_unit_sphere() -> Vec3 {
	let mut p: Vec3;
	loop {
		let r1 = random_f();
		let r2 = random_f();
		let r3 = random_f();
		p = 2.0 * Vec3([r1, r2, r3]) - Vec3([1., 1., 1.]);
		if p.squared_length() < 1.0 {
			break;
		}
	}
	p
}

fn color(r: Ray, world: &[HSphere]) -> Vec3 {
	let mut rec = HitRec{t: 0., p: Vec3([0., 0., 0.]), normal: Vec3([0., 0., 0.])};
	if hit(world, r, 0.001, 99999., &mut rec) {
//		println!("t={}", rec.t);
		let target = rec.normal + random_in_unit_sphere();
		0.5 * color(Ray{origin: rec.p, direction: target}, world)
	} else {
		let unit_direction = unit_vector(r.direction);
		let t = 0.5 * (unit_direction.0[1] + 1.);
		(1. - t) * Vec3([1., 1., 1.]) + t * Vec3([0.5, 0.7, 1.])
	}
}

fn main() {
	pcg::srand(0);
	let nx = 200;
	let ny = 100;
	let ns = 100;
	println!("P3");
	println!("{} {}", nx, ny);
	println!("255");
	let world = [
		HSphere{center: Vec3([0., 0., -1.]), radius: 0.5},
		HSphere{center: Vec3([0., -100.5, -1.]), radius: 100.},
	];
	let cam = Camera{
		lower_left_corner: Vec3([-2.0, -1.0, -1.0]),
		horizontal: Vec3([4., 0., 0.]),
		vertical: Vec3([0., 2., 0.]),
		origin: Vec3([0., 0., 0.]),
	};
//	println!("{}", cam);
//	_wprint(&world);
	for j in (0..ny).rev() {
		for i in 0..nx {
			let mut col = Vec3([0., 0., 0.]);
			for _s in 0..ns {
				let u = (i as f32 + random_f()) / nx as f32;
				let v = (j as f32 + random_f()) / ny as f32;
				let r = cam.get_ray(u, v);
//				println!("r={}", r);
				col = col + color(r, &world);
//				println!("col={} ", col);
			}
			col = col / ns as f32;
			col = Vec3([f32::sqrt(col.0[0]), f32::sqrt(col.0[1]), f32::sqrt(col.0[2])]);
			let _ir = (255.99*col.0[0]) as i32;
			let _ig = (255.99*col.0[1]) as i32;
			let _ib = (255.99*col.0[2]) as i32;
			println!("{} {} {}", _ir, _ig, _ib);
		}
	}
}
