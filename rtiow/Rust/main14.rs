mod pcg;

use std::env;
use std::fs::File;
use std::ops::Div;
use std::ops::Mul;
use std::fmt;
use std::io::Write;

const FLT_MAX: f32 = 340282346638528859811704183484516925440.000000;

macro_rules! vec3 {
    ($x:expr, $y:expr, $z:expr) => {
        Vec3 { x: $x, y: $y, z: $z }
    };
}

enum Material {
	Lambertian(Vec3),
	Metal(Vec3, f32),
	Dielectric(f32)
}

enum Hittable {
	Sphere(Vec3, f32, Material)
}

#[derive(Copy, Clone)]
struct Vec3 {
	x: f32,
	y: f32,
	z: f32
}

struct Ray {
	origin: Vec3,
	direction: Vec3
}

struct HitRec<'a> {
	t: f32,
	p: Vec3,
	normal: Vec3,
	mat: &'a Material
}

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

impl Material {
	fn scatter(&self, ray_in: &Ray, rec: &HitRec, attenuation: &mut Vec3, scattered: &mut Ray) -> bool {
		match *self {
			Material::Lambertian(albedo) => {
				let target = rec.normal + random_in_unit_sphere();
				*scattered = Ray{origin: rec.p, direction: target};
				*attenuation = albedo;
				true
			},
			Material::Metal(albedo, fuzz) => {
				let reflected = vreflect(unit_vector(ray_in.direction), rec.normal);
				*scattered = Ray{origin: rec.p, direction: reflected + fuzz * random_in_unit_sphere()};
				*attenuation = albedo;
				vdot(scattered.direction, rec.normal) > 0.
			},
			Material::Dielectric(ref_idx) => {
				let reflected = vreflect(ray_in.direction, rec.normal);
				*attenuation = vec3! {1., 1., 1.};
				let mut refracted = vec3! {0., 0., 0.};
				let mut reflect_prob = 1.0f32;
				let mut outward_normal = rec.normal;
				let mut ni_over_nt = 1. / ref_idx;
				let mut cosine = -vdot(ray_in.direction, rec.normal) / vlen(ray_in.direction);
				if vdot(ray_in.direction, rec.normal) > 0. {
					outward_normal = -rec.normal;
					ni_over_nt = ref_idx;
					cosine = ref_idx * vdot(ray_in.direction, rec.normal) / vlen(ray_in.direction);
				}
				if refract(ray_in.direction, outward_normal, ni_over_nt, &mut refracted) {
					reflect_prob = schlick(cosine, ref_idx);
				}
				if random_f() < reflect_prob {
					*scattered = Ray{origin: rec.p, direction: reflected};
				} else {
					*scattered = Ray{origin: rec.p, direction: refracted};
				}
				true
			}
		}
	}

	fn print(&self) {
		match *self {
			Material::Lambertian(albedo) => {
				print!("{{ML:{} }}", albedo);
			},
			Material::Metal(albedo, _) => {
				print!("{{MM:{} }}", albedo);
			},
			Material::Dielectric(ref_idx) => {
				print!("{{MD:{} }}", ref_idx);
			}
		}
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

impl Hittable {
	fn hit<'a>(&'a self, r: &Ray, t_min: f32, t_max: f32, rec: &mut HitRec<'a>) -> bool {
		match self {
			Hittable::Sphere(center, radius, material) => {
				let center = *center;
				let radius = *radius;
				let oc = r.origin - center;
//		println!("oc={:?}", oc);
				let a = r.direction % r.direction;
				let b = oc % r.direction;
				let c = (oc % oc) - radius * radius;
				let discriminant = b * b - a * c;
//		let abc = vec3! {a, b, c};
//		println!("abc={}", abc);
//		println!("a={} b={} c={} d={}", a, b, c, discriminant);
				if discriminant > 0. {
					let mut temp = (-b - f32::sqrt(discriminant)) / a;
					if temp < t_max && temp > t_min {
						rec.t = temp;
						rec.p = r.point_at_parameter(rec.t);
						rec.normal = (rec.p - center) / radius;
						rec.mat = &material;
						return true;
					}
					temp = (-b + f32::sqrt(discriminant)) / a;
					if temp < t_max && temp > t_min {
						rec.t = temp;
						rec.p = r.point_at_parameter(rec.t);
						rec.normal = (rec.p - center) / radius;
						rec.mat = &material;
						return true;
					}
				}
				false
			}
		}
	}

	fn print(&self) {
		match self {
			Hittable::Sphere(center, radius, material) => {
				print!("{{HS:{} ,{:.6},", center, radius);
				material.print();
				print!("}}");
			}
		}
	}
}

impl Camera {
	fn get_ray(&self, s: f32, t: f32) -> Ray {
		let rd = self.lens_radius * random_in_unit_disk();
		let offset = rd.x * self.u + rd.y * self.v;
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
			self.x, self.y, self.z,
			self.x.to_bits(), self.y.to_bits(), self.z.to_bits())
	}
}

impl Vec3 {
	fn squared_length(self) -> f32 {
		self.x * self.x + self.y * self.y + self.z * self.z
	}
	fn length(self) -> f32 {
		f32::sqrt(self.squared_length())
	}
}

impl std::ops::Rem for Vec3 {
	type Output = f32;

	fn rem(self, rhs: Vec3) -> f32 {
		self.x * rhs.x + self.y * rhs.y + self.z * rhs.z
	}
}

impl std::ops::Div for Vec3 {
	type Output = Vec3;

	fn div(self, rhs: Vec3) -> Self::Output {
		Self{x: self.x / rhs.x, y: self.y / rhs.y, z: self.z / rhs.z}
	}
}

impl std::ops::Mul for Vec3 {
	type Output = Vec3;

	fn mul(self, rhs: Vec3) -> Self::Output {
		Self{x: self.x * rhs.x, y: self.y * rhs.y, z: self.z * rhs.z}
	}
}

impl std::ops::Add for Vec3 {
	type Output = Vec3;

	fn add(self, rhs: Vec3) -> Self::Output {
		Self{x: self.x + rhs.x, y: self.y + rhs.y, z: self.z + rhs.z}
	}
}

impl std::ops::Sub for Vec3 {
	type Output = Vec3;

	fn sub(self, rhs: Vec3) -> Self::Output {
		Self{x: self.x - rhs.x, y: self.y - rhs.y, z: self.z - rhs.z}
	}
}

impl std::ops::Neg for Vec3 {
	type Output = Vec3;

	fn neg(self) -> Self::Output {
		Self{x: -self.x, y: -self.y, z: -self.z}
	}
}

impl Div<f32> for Vec3 {
	type Output = Self;

	fn div(self, rhs: f32) -> Self::Output {
		Self{ x: self.x / rhs, y: self.y / rhs, z: self.z / rhs}
	}
}

impl Mul<Vec3> for f32 {
	type Output = Vec3;

	fn mul(self, rhs: Vec3) -> Vec3 {
		vec3! {self * rhs.x, self * rhs.y, self * rhs.z }
	}
}

fn vlen(v: Vec3) -> f32 {
	f32::sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
}

fn unit_vector(v: Vec3) -> Vec3 {
	v / v.length()
}

fn vdot(v1: Vec3, v2: Vec3) -> f32 {
	v1.x * v2.x + v1.y * v2.y + v1.z * v2.z
}

fn vcross(v1: Vec3, v2: Vec3) -> Vec3 {
	vec3! {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	}
}

fn vreflect(v: Vec3, n: Vec3) -> Vec3 {
	v - 2.0 * vdot(v, n) * n
}

impl Ray {
	fn point_at_parameter(&self, t: f32) -> Vec3 {
		self.origin + t * self.direction
	}
}

fn random_in_unit_sphere() -> Vec3 {
	let mut p: Vec3;
	loop {
		let r1 = random_f();
		let r2 = random_f();
		let r3 = random_f();
		p = 2.0 * vec3! {r1, r2, r3} - vec3! {1., 1., 1.};
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
		p = 2.0 * vec3! {r1, r2, 0.} - vec3! {1., 1., 0.};
		if p.squared_length() < 1.0 {
			break;
		}
	}
	p
}

fn _wprint(world: &Vec<Hittable>) {
	println!("[");
	for h in world {
		h.print();
		println!(",");
	}
	println!("]");
}

fn hit<'a>(world: &'a Vec<Hittable>, r: &Ray, t_min: f32, t_max: f32, rec: &mut HitRec<'a>) -> bool {
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

fn color(world: &Vec<Hittable>, r: &Ray, depth: u32) -> Vec3 {
	let mut rec = HitRec{t: 0., p: vec3! {0., 0., 0.}, normal: vec3! {0., 0., 0.}, mat: &Material::Lambertian{0: vec3! {0., 0., 0.}}};
if cfg!(DEBUG) {
	println!("{}", r);
}
	if hit(world, r, 0.001, FLT_MAX, &mut rec) {
if cfg!(DEBUG) {
		print!("HIT\n");
		println!("t={:.6}", rec.t);
		print!("mat=");rec.mat.print();println!();
}
		let mut scattered = Ray{origin: vec3! {0., 0., 0.}, direction: vec3! {0., 0., 0.}};
		let mut attenuation = vec3! {0., 0., 0.};
		if depth < 50 && rec.mat.scatter(r, &rec, &mut attenuation, &mut scattered) {
if cfg!(DEBUG) {
			print!("ATT\n");
			let tv = vec3! {rec.t, 0., 0.};
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
			vec3! {0., 0., 0.}
		}
	} else {
		let unit_direction = unit_vector(r.direction);
if cfg!(DEBUG) {
		print!("NOT HIT");
		print!(" dir={}", r.direction);
		print!(" ud={}", unit_direction);
		print!(" \n");
}
		let t = 0.5 * (unit_direction.y + 1.);
		(1. - t) * vec3! {1., 1., 1.} + t * vec3! {0.5, 0.7, 1.}
	}
}

fn make_camera(lookfrom: Vec3, lookat: Vec3, vup: Vec3, vfov: f32, aspect: f32, aperture: f32, focus_dist: f32) -> Camera {
	let theta = vfov * std::f32::consts::PI / 180.0f32;
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

fn random_scene() -> Vec<Hittable> {
	let mut world: Vec<Hittable> = vec!(
		Hittable::Sphere {0: vec3! {0., -1000., 0.}, 1: 1000., 2: Material::Lambertian {0: vec3! {0.5, 0.5, 0.5}}},
	);
	for a in -11..11 {
		for b in -11..11 {
			let choose_mat = random_f();
			let r1 = random_f();
			let r2 = random_f();
if cfg!(DEBUG) {
			let crr = vec3! {choose_mat, r1, r2};
			println!("crr={} ", crr);
			println!("a={:.6} b={:.6}", a, b);
}
			let center = vec3! {a as f32 + 0.9 * r1, 0.2, b as f32 + 0.9 * r2};
if cfg!(DEBUG) {
			println!("center={} ", center);
}
			if vlen(center - vec3! {4., 0.2, 0.}) > 0.9 {
				if choose_mat < 0.8 {  // diffuse
					let r1 = random_f();
					let r2 = random_f();
					let r3 = random_f();
					let r4 = random_f();
					let r5 = random_f();
					let r6 = random_f();
	world.push(
		Hittable::Sphere {0: center, 1: 0.2, 2: Material::Lambertian {0: vec3! {r1 * r2, r3 * r4, r5 * r6}}},
	);
				} else if choose_mat < 0.95 { // metal
					let r1 = 0.5 * (1. + random_f());
					let r2 = 0.5 * (1. + random_f());
					let r3 = 0.5 * (1. + random_f());
					let r4 = 0.5 * random_f();
	world.push(
		Hittable::Sphere {0: center, 1: 0.2, 2: Material::Metal {0: vec3! {r1, r2, r3}, 1: r4}},
	);
				} else {  // glass
	world.push(
		Hittable::Sphere {0: center, 1: 0.2, 2: Material::Dielectric {0: 1.5}},
	);
				}
			}
		}
	}
	world.push(
		Hittable::Sphere {0: vec3! {0., 1., 0.}, 1: 1., 2: Material::Dielectric {0: 1.5}},
	);
	world.push(
		Hittable::Sphere {0: vec3! {-4., 1., 0.}, 1: 1., 2: Material::Lambertian {0: vec3! {0.4, 0.2, 0.1} }},
	);
	world.push(
		Hittable::Sphere {0: vec3! {4., 1., 0.}, 1: 1., 2: Material::Metal {0: vec3! {0.7, 0.6, 0.5}, 1: 0.}},
	);
	world
}

fn main() {
	pcg::srand(0);
	let mut fnameout = "";
	let mut nx = 200;
	let mut ny = 100;
	let mut ns = 1;
	let mut arg = 0;
	let args : Vec<String> = env::args().collect();
	if arg < args.len() - 1 {
		arg += 1;
		nx = args[arg].parse().unwrap();
		if arg < args.len() - 1 {
			arg += 1;
			ny = args[arg].parse().unwrap();
			if arg < args.len() - 1 {
				arg += 1;
				ns = args[arg].parse().unwrap();
				if arg < args.len() - 1 {
					arg += 1;
					fnameout = &args[arg];
				}
			}
		}
	}
	let foutopt = if fnameout != "" {
		Some(File::create(fnameout).expect("Can't create file"))
	} else {
		None
	};
	let mut bytes = if fnameout != "" {
		vec![0u8; 3 * ny * nx]
	} else {
		vec![0u8; 0]
	};
	if fnameout == "" {
		println!("P3");
		println!("{} {}", nx, ny);
		println!("255");
	}
	let world = random_scene();
	let lookfrom = vec3! {9., 2., 2.6};
	let lookat = vec3! {3., 0.8, 1.};
	let dist_to_focus = vlen(lookfrom - lookat);
	let aperture = 0.0f32;
	let cam = make_camera(lookfrom, lookat, vec3! {0.,1.,0.}, 30., nx as f32 / ny as f32, aperture, dist_to_focus);
if cfg!(DEBUG) {
	println!("{}", cam);
	_wprint(&world);
}
	for j in (0..ny).rev() {
		for i in 0..nx {
			let mut col = vec3! {0., 0., 0.};
			for _s in 0..ns {
				let u = (i as f32 + random_f()) / nx as f32;
				let v = (j as f32 + random_f()) / ny as f32;
				let r = cam.get_ray(u, v);
if cfg!(DEBUG) {
				println!("r={}", r);
}
				col = col + color(&world, &r, 0);
if cfg!(DEBUG) {
				println!("col={}", col);
}
			}
			col = col / ns as f32;
			col = vec3! {f32::sqrt(col.x), f32::sqrt(col.y), f32::sqrt(col.z)};
			let ir = (255.99*col.x) as u8;
			let ig = (255.99*col.y) as u8;
			let ib = (255.99*col.z) as u8;
			if fnameout != "" {
				bytes[((ny - 1 - j) * nx + i) * 3 + 0] = ir;
				bytes[((ny - 1 - j) * nx + i) * 3 + 1] = ig;
				bytes[((ny - 1 - j) * nx + i) * 3 + 2] = ib;
			} else {
				print!("{} {} {}  ", ir, ig, ib);
			}
		}
		if fnameout == "" {
			println!();
		}
	}
	match foutopt {
		Some(mut fout) => {
			writeln!(fout, "P6").expect("Can't write");
			writeln!(fout, "{} {}", nx, ny).expect("Can't write");
			writeln!(fout, "255").expect("Can't write");
			fout.write(&bytes).expect("Can't write");
		}
		None => {}
	}
}
