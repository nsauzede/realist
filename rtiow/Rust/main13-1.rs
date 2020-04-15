mod pcg;

use std::ops::Div;
use std::ops::Mul;
use std::fmt;

const FLT_MAX: f32 = 340282346638528859811704183484516925440.000000;

trait Hittable {
	fn hit<'a>(&'a self, r: &Ray, t_min: f32, t_max: f32, rec: &mut HitRec<'a>) -> bool;
	fn print(&self);
}

trait Material {
	fn scatter(&self, ray_in: &Ray, rec: &HitRec, attenuation: &mut Vec3, scattered: &mut Ray) -> bool;
	fn print(&self);
}

#[derive(Copy, Clone)]
struct Vec3([f32; 3]);

#[derive(Copy, Clone)]
struct Ray {
	origin: Vec3,
	direction: Vec3
}

struct Lambertian {
	albedo: Vec3,
}

struct Metal {
	albedo: Vec3,
	fuzz: f32
}

struct Dielectric {
	ref_idx: f32
}

struct Sphere {
	center: Vec3,
	radius: f32,
	material: Box<dyn Material>
}

struct HitRec<'a> {
	t: f32,
	p: Vec3,
	normal: Vec3,
	mat: &'a dyn Material
}

#[derive(Copy, Clone)]
struct Camera {
	origin: Vec3,
	lower_left_corner: Vec3,
	horizontal: Vec3,
	vertical: Vec3,
	u: Vec3,
	v: Vec3,
	w: Vec3,
	lens_radius: f32
}

fn random_f() -> f32 {
	pcg::rand() as f32 / (pcg::RAND_MAX as f32 + 1.0)
}

impl Material for Lambertian {
	fn scatter(&self, _ray_in: &Ray, rec: &HitRec, attenuation: &mut Vec3, scattered: &mut Ray) -> bool {
		let target = rec.normal + random_in_unit_sphere();
		*scattered = Ray{origin: rec.p, direction: target};
		*attenuation = self.albedo;
		true
	}
	fn print(&self) {
		print!("{{ML:{} }}", self.albedo);
	}
}

impl Material for Metal {
	fn scatter(&self, ray_in: &Ray, rec: &HitRec, attenuation: &mut Vec3, scattered: &mut Ray) -> bool {
		let reflected = vreflect(unit_vector(ray_in.direction), rec.normal);
		*scattered = Ray{origin: rec.p, direction: reflected + self.fuzz * random_in_unit_sphere()};
		*attenuation = self.albedo;
		vdot(scattered.direction, rec.normal) > 0.
	}
	fn print(&self) {
		print!("{{MM:{} }}", self.albedo);
	}
}

fn refract(v: Vec3, n: Vec3, ni_over_nt: f32, refracted: &mut Vec3) -> bool {
	let uv = unit_vector(v);
	let dt = vdot(uv, n);
	let discriminant = 1. - ni_over_nt * ni_over_nt * (1. - dt * dt);
	if discriminant > 0. {
		*refracted = ni_over_nt * (uv - dt * n) - f32::sqrt(discriminant) * n;
		true
	} else {
		false
	}
}

fn schlick(cosine: f32, ref_idx: f32) -> f32 {
	let mut r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
	r0 = r0 * r0;
	r0 + (1.0 - r0) * f32::powf(1.0 - cosine, 5.0)
}

impl Material for Dielectric {
	fn scatter(&self, ray_in: &Ray, rec: &HitRec, attenuation: &mut Vec3, scattered: &mut Ray) -> bool {
		let reflected = vreflect(ray_in.direction, rec.normal);
		*attenuation = Vec3([1., 1., 1.]);
		let mut refracted = Vec3([0., 0., 0.]);
		let mut reflect_prob = 1 as f32;
		let mut outward_normal = rec.normal;
		let mut ni_over_nt = 1. / self.ref_idx;
		let mut cosine = -vdot(ray_in.direction, rec.normal) / vlen(ray_in.direction);
		if vdot(ray_in.direction, rec.normal) > 0. {
			outward_normal = -rec.normal;
			ni_over_nt = self.ref_idx;
			cosine = self.ref_idx * vdot(ray_in.direction, rec.normal) / vlen(ray_in.direction);
		}
		if refract(ray_in.direction, outward_normal, ni_over_nt, &mut refracted) {
			reflect_prob = schlick(cosine, self.ref_idx);
		}
		if random_f() < reflect_prob {
			*scattered = Ray{origin: rec.p, direction: reflected};
		} else {
			*scattered = Ray{origin: rec.p, direction: refracted};
		}
		return true;
	}
	fn print(&self) {
		print!("{{MD:{} }}", self.ref_idx);
	}
}

impl Hittable for Sphere {
	fn hit<'a>(&'a self, r: &Ray, t_min: f32, t_max: f32, rec: &mut HitRec<'a>) -> bool {
		let center = self.center;
		let radius = self.radius;
		let oc = r.origin - center;
//		println!("oc={:?}", oc);
		let a = r.direction % r.direction;
		let b = oc % r.direction;
		let c = (oc % oc) - radius * radius;
		let discriminant = b * b - a * c;
//		let abc = Vec3([a, b, c]);
//		println!("abc={}", abc);
//		println!("a={} b={} c={} d={}", a, b, c, discriminant);
		if discriminant > 0. {
			let mut temp = (-b - f32::sqrt(discriminant)) / a;
			if temp < t_max && temp > t_min {
				rec.t = temp;
				rec.p = r.point_at_parameter(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.mat = self.material.as_ref();
				return true;
			}
			temp = (-b + f32::sqrt(discriminant)) / a;
			if temp < t_max && temp > t_min {
				rec.t = temp;
				rec.p = r.point_at_parameter(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.mat = self.material.as_ref();
				return true;
			}
		}
		false
	}
	fn print(&self) {
		print!("{{HS:{} ,{:.6},", self.center, self.radius);
		self.material.print();
		print!("}}");
	}
}

impl Camera {
	fn get_ray(self, s: f32, t: f32) -> Ray {
		let rd = self.lens_radius * random_in_unit_disk();
		let offset = rd.0[0] * self.u + rd.0[1] * self.v;
		Ray{
			origin: self.origin + offset,
			direction: self.lower_left_corner + s * self.horizontal + t * self.vertical - self.origin - offset
		}
	}
}

impl fmt::Display for Camera {
	fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
		write!(f, "{{\n\tlower_left_corner: {} \n\thorizontal: {} \n\tvertical: {} \n\torigin: {} \n}}\nu: {}\nv: {}\nw: {}\nlens_radius={:.6}\n",
			self.lower_left_corner,
			self.horizontal,
			self.vertical,
			self.origin,
			self.u,
			self.v,
			self.w,
			self.lens_radius)
	}
}

impl fmt::Display for Ray {
	fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
		write!(f, "{{{}, {}}}",
			self.origin,
			self.direction)
	}
}

impl fmt::Display for Vec3 {
	fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
		write!(f, "{{{:.6}, {:.6}, {:.6};{:x}, {:x}, {:x}}}",
			self.0[0], self.0[1], self.0[2],
			self.0[0].to_bits(), self.0[1].to_bits(), self.0[2].to_bits())
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

impl std::ops::Mul for Vec3 {
	type Output = Vec3;

	fn mul(self, rhs: Vec3) -> Self::Output {
		let Self([x1, y1, z1]) = self;
		let Self([x2, y2, z2]) = rhs;

		Self([x1 * x2, y1 * y2, z1 * z2])
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

impl std::ops::Neg for Vec3 {
	type Output = Vec3;

	fn neg(self) -> Self::Output {
		let Self([x1, y1, z1]) = self;

		Self([-x1, -y1, -z1])
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

fn vlen(v: Vec3) -> f32 {
	f32::sqrt(v.0[0] * v.0[0] + v.0[1] * v.0[1] + v.0[2] * v.0[2])
}

fn unit_vector(v: Vec3) -> Vec3 {
	v / v.length()
}

fn vdot(v1: Vec3, v2: Vec3) -> f32 {
	v1.0[0] * v2.0[0] + v1.0[1] * v2.0[1] + v1.0[2] * v2.0[2]
}

fn vcross(v1: Vec3, v2: Vec3) -> Vec3 {
	Vec3([
		v1.0[1] * v2.0[2] - v1.0[2] * v2.0[1],
		v1.0[2] * v2.0[0] - v1.0[0] * v2.0[2],
		v1.0[0] * v2.0[1] - v1.0[1] * v2.0[0]
	])
}

fn vreflect(v: Vec3, n: Vec3) -> Vec3 {
	v - 2.0 * vdot(v, n) * n
}

impl Ray {
	fn point_at_parameter(self, t: f32) -> Vec3 {
		self.origin + t * self.direction
	}
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

fn random_in_unit_disk() -> Vec3 {
	let mut p: Vec3;
	loop {
		let r1 = random_f();
		let r2 = random_f();
		p = 2.0 * Vec3([r1, r2, 0.]) - Vec3([1., 1., 0.]);
		if p.squared_length() < 1.0 {
			break;
		}
	}
	p
}

fn _wprint(world: &Vec<Box<dyn Hittable>>) {
	println!("[");
	for h in world {
		h.print();
		println!(",");
	}
	println!("]");
}

fn hit<'a>(world: &'a Vec<Box<dyn Hittable>>, r: &Ray, t_min: f32, t_max: f32, rec: &mut HitRec<'a>) -> bool {
	let mut hit_anything = false;
	let mut closest_so_far = t_max;
	for h in world {
		if h.hit(r, t_min, closest_so_far, rec) {
			hit_anything = true;
			closest_so_far = rec.t;
		}
	}
	hit_anything
}

fn color(world: &Vec<Box<dyn Hittable>>, r: &Ray, depth: u32) -> Vec3 {
	let mut rec = HitRec{t: 0., p: Vec3([0., 0., 0.]), normal: Vec3([0., 0., 0.]), mat: &Lambertian{albedo: Vec3([0., 0., 0.])}};
if cfg!(DEBUG) {
	println!("{}", r);
}
	if hit(world, r, 0.001, FLT_MAX, &mut rec) {
if cfg!(DEBUG) {
		print!("HIT\n");
		println!("t={:.6}", rec.t);
		print!("mat=");rec.mat.print();println!();
}
		let mut scattered = Ray{origin: Vec3([0., 0., 0.]), direction: Vec3([0., 0., 0.])};
		let mut attenuation = Vec3([0., 0., 0.]);
		if depth < 50 && rec.mat.scatter(r, &rec, &mut attenuation, &mut scattered) {
if cfg!(DEBUG) {
			print!("ATT\n");
			let tv = Vec3([rec.t, 0., 0.]);
			print!("tv={}", tv);
			print!(" \np={}", rec.p);
			print!(" \n");
			print!("nor={}", rec.normal);
//			print!(" \n");
//			print!("h={}", h);
			print!("\nsca={}", scattered);
			print!(" \n");
}
			attenuation * color(world, &scattered, depth + 1)
		} else {
if cfg!(DEBUG) {
			print!("NOT ATT\n");
}
			Vec3([0., 0., 0.])
		}
	} else {
		let unit_direction = unit_vector(r.direction);
if cfg!(DEBUG) {
		print!("NOT HIT");
		print!(" dir={}", r.direction);
		print!(" ud={}", unit_direction);
		print!(" \n");
}
		let t = 0.5 * (unit_direction.0[1] + 1.);
		(1. - t) * Vec3([1., 1., 1.]) + t * Vec3([0.5, 0.7, 1.])
	}
}

fn make_camera(lookfrom: Vec3, lookat: Vec3, vup: Vec3, vfov: f32, aspect: f32, aperture: f32, focus_dist: f32) -> Camera {
	let theta = vfov * std::f32::consts::PI / 180 as f32;
	let half_height = f32::tan(theta / 2.);
	let half_width = aspect * half_height;
	let w = unit_vector(lookfrom - lookat);
	let u = unit_vector(vcross(vup, w));
	let v = vcross(w, u);
	Camera {
		origin: lookfrom,
		lower_left_corner: lookfrom - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w,
		horizontal: 2. * half_width * focus_dist * u,
		vertical: 2. * half_height * focus_dist * v,
		u: u,
		v: v,
		w: w,
		lens_radius: aperture / 2.
	}
}

fn random_scene() -> Vec<Box<dyn Hittable>> {
	let mut world: Vec<Box<dyn Hittable>> = vec!(
		Box::new(Sphere {center: Vec3([0., -1000., 0.]), radius: 1000., material: Box::new(Lambertian {albedo: Vec3([0.5, 0.5, 0.5])})}),
	);
	for a in -11..11 {
		for b in -11..11 {
			let choose_mat = random_f();
			let r1 = random_f();
			let r2 = random_f();
if cfg!(DEBUG) {
			let crr = Vec3([choose_mat, r1, r2]);
			println!("crr={} ", crr);
			println!("a={:.6} b={:.6}", a, b);
}
			let center = Vec3([a as f32 + 0.9 * r1, 0.2, b as f32 + 0.9 * r2]);
if cfg!(DEBUG) {
			println!("center={} ", center);
}
			if vlen(center - Vec3([4., 0.2, 0.])) > 0.9 {
				if choose_mat < 0.8 {  // diffuse
					let r1 = random_f();
					let r2 = random_f();
					let r3 = random_f();
					let r4 = random_f();
					let r5 = random_f();
					let r6 = random_f();
	world.push(
		Box::new(Sphere {center: center, radius: 0.2, material: Box::new(Lambertian {albedo: Vec3([r1 * r2, r3 * r4, r5 * r6])})}),
	);
				} else if choose_mat < 0.95 { // metal
					let r1 = 0.5 as f32 * (1. + random_f());
					let r2 = 0.5 * (1. + random_f());
					let r3 = 0.5 * (1. + random_f());
					let r4 = 0.5 * random_f();
	world.push(
		Box::new(Sphere {center: center, radius: 0.2, material: Box::new(Metal {albedo: Vec3([r1, r2, r3]), fuzz: r4})}),
	);
				} else {  // glass
	world.push(
		Box::new(Sphere {center: center, radius: 0.2, material: Box::new(Dielectric {ref_idx: 1.5})}),
	);
				}
			}
		}
	}
	world.push(
		Box::new(Sphere {center: Vec3([0., 1., 0.]), radius: 1., material: Box::new(Dielectric {ref_idx: 1.5})}),
	);
	world.push(
		Box::new(Sphere {center: Vec3([-4., 1., 0.]), radius: 1., material: Box::new(Lambertian {albedo: Vec3([0.4, 0.2, 0.1]) })}),
	);
	world.push(
		Box::new(Sphere {center: Vec3([4., 1., 0.]), radius: 1., material: Box::new(Metal {albedo: Vec3([0.7, 0.6, 0.5]), fuzz: 0.})}),
	);
	world
}

fn main() {
	pcg::srand(0);
	let nx = 200;
	let ny = 100;
	let ns = 1;
	println!("P3");
	println!("{} {}", nx, ny);
	println!("255");
	let world = random_scene();
	let lookfrom = Vec3([9., 2., 2.6]);
	let lookat = Vec3([3., 0.8, 1.]);
	let dist_to_focus = vlen(lookfrom - lookat);
	let aperture = 0 as f32;
	let cam = make_camera(lookfrom, lookat, Vec3([0.,1.,0.]), 30., nx as f32 / ny as f32, aperture, dist_to_focus);
//if cfg!(DEBUG) {
	println!("{}", cam);
//	_wprint(&world);
//}
	for j in (0..ny).rev() {
		for i in 0..nx {
			let mut col = Vec3([0., 0., 0.]);
			for _s in 0..ns {
				let u = (i as f32 + random_f()) / nx as f32;
				let v = (j as f32 + random_f()) / ny as f32;
//if cfg!(DEBUG) {
				let uv = Vec3([u, v, 0.]);
				println!("uv={} ", uv);
//}
				let r = cam.get_ray(u, v);
//if cfg!(DEBUG) {
				println!("r={}", r);
//}
				col = col + color(&world, &r, 0);
if cfg!(DEBUG) {
				println!("col={}", col);
}
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
