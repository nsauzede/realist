package main

import "core:fmt"
import "core:math"

import "pcg"

FLT_MAX : f32 = 340282346638528859811704183484516925440.000000;

rfcnt := u32(0);
riuscnt := u32(0);
riudcnt := u32(0);

random_f :: proc() -> f32 {
when #defined(DEBUG) {
	rfcnt += 1;
}
	return f32(pcg.rand()) / (f32(pcg.RAND_MAX) + 1.0);
}

random_in_unit_sphere :: proc() -> Vec3 {
when #defined(DEBUG) {
	riuscnt += 1;
}
	p := Vec3{};
	for {
		r1 := random_f();
		r2 := random_f();
		r3 := random_f();
		p = 2.0 * Vec3{r1, r2, r3} - Vec3{1,1,1};
		if vsqlen(p) < 1.0 {
			break;
		}
	}
	return p;
}

random_in_unit_disk :: proc() -> Vec3 {
when #defined(DEBUG) {
	riudcnt += 1;
}
	p := Vec3{};
	for {
		r1 := random_f();
		r2 := random_f();
		p = 2.0 * Vec3{r1, r2, 0} - Vec3{1,1,0};
		if vsqlen(p) < 1.0 {
			break;
		}
	}
	return p;
}

Vec3 :: [3]f32;

Ray :: struct {
	origin: Vec3,
	direction: Vec3
}

Material :: union {
	Material_Metal,
	Material_Lambertian,
	Material_Dielectric,
}

Material_Metal :: struct {
	albedo: [3]f32,
	fuzz: f32
}

Material_Lambertian :: struct {
	albedo: [3]f32,
}

Hittable :: union {
	Sphere,
}

Material_Dielectric :: struct {
	ref_idx: f32
}

Sphere :: struct {
	center: Vec3,
	radius: f32,
	material: Material
}

Hit_Record :: struct {
	t: f32,
	p: Vec3,
	normal: Vec3,
	mat: Material,
	h_ptr: ^Hittable
}

Camera :: struct {
	lower_left_corner: Vec3,
	horizontal: Vec3,
	vertical: Vec3,
	origin: Vec3,
	u: Vec3,
	v: Vec3,
	w: Vec3,
	lens_radius: f32
}

point_at_parameter :: proc(r: Ray, t: f32) -> Vec3 {
	return r.origin + t * r.direction;
}

vlen :: proc(v: Vec3) -> f32 {
	return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

vsqlen :: proc(v: Vec3) -> f32 {
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

unit_vector :: proc(v: Vec3) -> Vec3 {
	return v / vlen(v);
}

vdot :: proc(v1: Vec3, v2: Vec3) -> f32 {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

vcross :: proc(v1: Vec3, v2: Vec3) -> Vec3 {
	return Vec3{
		v1[1] * v2[2] - v1[2] * v2[1],
		v1[2] * v2[0] - v1[0] * v2[2],
		v1[0] * v2[1] - v1[1] * v2[0]};
}

vreflect :: proc(v: Vec3, n: Vec3) -> Vec3 {
	return v - 2.0 * vdot(v, n) * n;
}

hit_sphere :: proc(s: Sphere, r: Ray, t_min: f32, t_max: f32, rec: ^Hit_Record) -> bool {
when #defined(DEBUG) {
	fmt.printf("ray=");rprint(r);fmt.printf(" \n");
}
	oc := r.origin - s.center;
when #defined(DEBUG) {
	fmt.printf("oc=");vprint(oc);fmt.printf(" \n");
}
	a := vdot(r.direction, r.direction);
	b := vdot(oc, r.direction);
	c := vdot(oc, oc) - s.radius * s.radius;
when #defined(DEBUG) {
	abc := Vec3{a, b, c};
	fmt.printf("abc=");vprint(abc);fmt.printf(" \n");
}
	discriminant := b * b - a * c;
	if discriminant > 0 {
		temp := (-b - math.sqrt(discriminant)) / a;
		if temp < t_max && temp > t_min {
			rec.t = temp;
			rec.p = point_at_parameter(r, rec.t);
			rec.normal = (rec.p - s.center) / s.radius;
			rec.mat = s.material;
			return true;
		}
		temp = (-b + math.sqrt(discriminant)) / a;
		if temp < t_max && temp > t_min {
			rec.t = temp;
			rec.p = point_at_parameter(r, rec.t);
			rec.normal = (rec.p - s.center) / s.radius;
			rec.mat = s.material;
			return true;
		}
	}
	return false;
}

hit :: proc(world: []Hittable, r: Ray, t_min: f32, t_max: f32, rec: ^Hit_Record) -> bool {
	hit_anything := false;
	closest_so_far := t_max;
when false {
	for hittable in world {
		switch h in hittable {
		case Sphere:
			if hit_sphere(h, r, t_min, closest_so_far, rec) {
				hit_anything = true;
				closest_so_far = rec.t;
				rec.h_ptr = &h;
			}
		}
	}
} else {
	for i := 0; i < len(world); i+= 1 {
		hittable := &world[i];
		switch h in hittable {
		case Sphere:
			if hit_sphere(h, r, t_min, closest_so_far, rec) {
				hit_anything = true;
				closest_so_far = rec.t;
				rec.h_ptr = hittable;
			}
		}
	}
}
	return hit_anything;
}

mprint :: proc(material: Material) {
	switch m in material {
	case Material_Metal:
		fmt.printf("{{MM:");vprint(m.albedo);fmt.printf(" ,%.6f}}", m.fuzz);
	case Material_Lambertian:
		fmt.printf("{{ML:");vprint(m.albedo);fmt.printf(" }}");
	case Material_Dielectric:
		fmt.printf("{{MD:%.6f}}", m.ref_idx);
	}
}

hprint :: proc(hittable: ^Hittable) {
	switch h in hittable {
	case Sphere:
		fmt.printf("{{HS:");vprint(h.center);fmt.printf(" ,%.6f,", h.radius);
		mprint(h.material);
		fmt.printf("}}");
	}
}

refract :: proc(v: Vec3, n: Vec3, ni_over_nt: f32, refracted: ^Vec3) -> bool {
	uv := unit_vector(v);
when #defined(DEBUG) {
	fmt.printf("refuv=");vprint(uv);fmt.printf(" \n");
	fmt.printf("refn=");vprint(n);fmt.printf(" \n");
}
	dt := vdot(uv, n);
	discriminant := 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
when #defined(DEBUG) {
	ddn := Vec3{dt, discriminant, ni_over_nt};
	fmt.printf("ddn=");vprint(ddn);fmt.printf(" \n");
}
	if discriminant > 0 {
		refracted^ = ni_over_nt * (uv - n * dt) - n * math.sqrt(discriminant);
		return true;
	} else {
		return false;
	}
}

schlick :: proc(cosine: f32, ref_idx: f32) -> f32 {
	r0 := (1.0 - ref_idx) / (1.0 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0 - r0) * math.pow(1.0 - cosine, 5.0);
}

scatter :: proc(material: Material, ray_in: Ray, rec: ^Hit_Record, attenuation: ^Vec3, scattered: ^Ray) -> bool {
	switch m in material {
	case Material_Metal:
		reflected := vreflect(unit_vector(ray_in.direction), rec.normal);
		scattered^ = Ray{rec.p, reflected + m.fuzz * random_in_unit_sphere()};
		attenuation^ = m.albedo;
		return vdot(scattered.direction, rec.normal) > 0;
	case Material_Lambertian:
		target := rec.normal + random_in_unit_sphere();
		scattered^ = Ray{rec.p, target};
		attenuation^ = m.albedo;
		return true;
	case Material_Dielectric:
		outward_normal := Vec3{};
		reflected := vreflect(ray_in.direction, rec.normal);
		ni_over_nt : f32 = 0;
		attenuation^ = Vec3{1, 1, 1};
		refracted := Vec3{};
		reflect_prob : f32;
		cosine : f32;
		dot := vdot(ray_in.direction, rec.normal);
		len := vlen(ray_in.direction);
		if dot > 0 {
			outward_normal = -rec.normal;
			ni_over_nt = m.ref_idx;
			cosine = m.ref_idx * dot / len;
		} else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / m.ref_idx;
			cosine = -dot / len;
		}
when #defined(DEBUG) {
		dln := Vec3{dot, len, ni_over_nt};
		fmt.printf("dln=");vprint(dln);fmt.printf(" \n");
}
		if refract(ray_in.direction, outward_normal, ni_over_nt, &refracted) {
when #defined(DEBUG) {
			fmt.printf("SCHLICK\n");
}
			reflect_prob = schlick(cosine, m.ref_idx);
		} else {
when #defined(DEBUG) {
			fmt.printf("NOSCHLICK\n");
}
			reflect_prob = 1.0;
		}
		if random_f() < reflect_prob {
when #defined(DEBUG) {
			fmt.printf("REFL\n");
}
			scattered^ = Ray{rec.p, reflected};
		} else {
when #defined(DEBUG) {
			fmt.printf("REFR\n");
}
			scattered^ = Ray{rec.p, refracted};
		}
		return true;
	}
	return false;
}

color :: proc(world: []Hittable, r: Ray, depth: int) -> Vec3 {
	rec := Hit_Record{};
	if hit(world, r, 0.001, FLT_MAX, &rec) {
when #defined(DEBUG) {
		fmt.printf("HIT\n");
}
		scattered := Ray{};
		attenuation := Vec3{};
		h := rec.h_ptr;
		if depth < 50 && scatter(rec.mat, r, &rec, &attenuation, &scattered) {
when #defined(DEBUG) {
			fmt.printf("ATT\n");
			tv := Vec3{rec.t, 0, 0};
                        fmt.printf("tv=");
                        vprint(tv);
                        fmt.printf(" \np=");
                        vprint(rec.p);
                        fmt.printf(" \n");
                        fmt.printf("nor=");
                        vprint(rec.normal);
                        fmt.printf(" \n");
                        fmt.printf("h=");
                        hprint(h);
                        fmt.printf("\nsca=");
//                        fmt.printf("sca=");
                        rprint(scattered);
                        fmt.printf(" \n");
}
			return attenuation * color(world, scattered, depth + 1);
		} else {
when #defined(DEBUG) {
			fmt.printf("NOT ATT\n");
}
			return Vec3{0, 0, 0};
		}
	} else {
		unit_direction := unit_vector(r.direction);
when #defined(DEBUG) {
		fmt.printf("NOT HIT");
                fmt.printf(" dir=");
                vprint(r.direction);
                fmt.printf(" ud=");
                vprint(unit_direction);
                fmt.printf(" \n");
}
		t := 0.5 * (unit_direction[1] + 1.0);
		return (1.0 - t) * Vec3{1.0, 1.0, 1.0} + t * Vec3{0.5, 0.7, 1.0};
	}
}

make_camera :: proc(lookfrom: Vec3, lookat: Vec3, vup: Vec3, vfov: f32, aspect: f32, aperture: f32, focus_dist: f32) -> Camera {
when #defined(DEBUG) {
//	vap := Vec3{vfov, aspect, math.PI};
//	fmt.printf("vap=");vprint(vap);fmt.printf(" \n");
}
	cam: Camera;
	theta := vfov * math.PI / 180;
//	half_height := math.tan(theta / f32(2));
	half_height := f32(math.tan(f64(theta) / 2));	// HACK : workaround to get same compute as C libm which has bug with tanf ?
	half_width := aspect * half_height;
when #defined(DEBUG) {
	thw := Vec3{theta, half_height, half_width};
	fmt.printf("thw=");vprint(thw);fmt.printf(" \n");
}
	cam.w = unit_vector(lookfrom - lookat);
	cam.u = unit_vector(vcross(vup, cam.w));
	cam.v = vcross(cam.w, cam.u);
	cam.origin = lookfrom;
	cam.lower_left_corner = lookfrom - half_width * focus_dist * cam.u - half_height * focus_dist * cam.v - focus_dist * cam.w;
	cam.horizontal = 2 * half_width * focus_dist * cam.u;
	cam.vertical = 2 * half_height * focus_dist * cam.v;
	cam.lens_radius = aperture / 2;
	return cam;
}

vprint :: proc(v: Vec3) {
	fmt.printf("{{%.6f, %.6f, %.6f;%x, %x, %x}}", v[0], v[1], v[2], transmute(u32)v[0], transmute(u32)v[1], transmute(u32)v[2]);
}

rprint :: proc(ray: Ray) {
	fmt.printf("{{");
	vprint(ray.origin);
	fmt.printf(", ");
	vprint(ray.direction);
	fmt.printf("}}");
}

cam_print :: proc(cam: Camera) {
	fmt.printf("{{\n\tlower_left_corner: ");vprint(cam.lower_left_corner);fmt.printf(" ");
	fmt.printf("\n\thorizontal: ");vprint(cam.horizontal);fmt.printf(" ");
	fmt.printf("\n\tvertical: ");vprint(cam.vertical);fmt.printf(" ");
	fmt.printf("\n\torigin: ");vprint(cam.origin);fmt.printf(" ");
	fmt.printf("\n}}\n");
        fmt.printf("\nu: ");vprint(cam.u);
        fmt.printf("\nv: ");vprint(cam.v);
        fmt.printf("\nw: ");vprint(cam.w);
        fmt.printf("\nlens_radius=%.6f\n", cam.lens_radius);
}

wprint :: proc(world: []Hittable) {
	fmt.printf("[\n");
	for hittable in world {
		switch h in hittable {
		case Sphere:
			fmt.printf("{{HS:");vprint(h.center);fmt.printf(" ,%.6f,", h.radius);
			mprint(h.material);
			fmt.printf("}}");
		}
		fmt.printf(",\n");
	}
	fmt.printf("]\n");
}

get_ray :: proc(c: Camera, s: f32, t: f32) -> Ray {
//	return Ray{c.origin, c.lower_left_corner + u * c.horizontal + v * c.vertical - c.origin};
//	st := Vec3{s, t, 0};
//	fmt.printf("st=");vprint(st);fmt.printf(" \n");
//	fmt.printf("vert=");vprint(c.vertical);fmt.printf(" \n");
	rd := c.lens_radius * random_in_unit_disk();
	offset := c.u * rd[0] + c.v * rd[1];
	return Ray{c.origin + offset, c.lower_left_corner + s * c.horizontal + t * c.vertical - c.origin - offset};
}

random_scene :: proc() -> [dynamic]Hittable {
	world : [dynamic]Hittable;
        append(&world, Sphere{Vec3{0, -1000, 0}, 1000, Material_Lambertian{Vec3{0.5, 0.5, 0.5}}});
        for a := -11; a < 11; a += 1 {
                for b := -11; b < 11; b += 1 {
                        choose_mat := random_f();
                        r1 := random_f();
                        r2 := random_f();
when #defined(DEBUG) {
                        fmt.printf("crr=");vprint(Vec3{choose_mat, r1, r2});fmt.printf(" \n");
                        fmt.printf("a=%d b=%d\n", a, b);
}
                        center := Vec3{f32(a) + 0.9 * r1, 0.2, f32(b) + 0.9 * r2};
when #defined(DEBUG) {
                        fmt.printf("center=");vprint(center);fmt.printf(" \n");
}
                        if vlen(center - Vec3{4, 0.2, 0}) > 0.9 {
                                if (choose_mat < 0.8) {  // diffuse
                                        r1 := random_f();
                                        r2 := random_f();
                                        r3 := random_f();
                                        r4 := random_f();
                                        r5 := random_f();
                                        r6 := random_f();
append(&world, Sphere{Vec3{center[0], center[1], center[2]}, 0.2, Material_Lambertian{Vec3{r1 * r2, r3 * r4, r5 * r6}}});
                                } else if (choose_mat < 0.95) { // metal
                                        r1 := f32(.5) * (1 + random_f());
                                        r2 := .5 * (1 + random_f());
                                        r3 := .5 * (1 + random_f());
                                        r4 := .5 * random_f();
append(&world, Sphere{Vec3{center[0], center[1], center[2]}, 0.2, Material_Metal{Vec3{r1, r2, r3}, r4}});
                                } else {  // glass
append(&world, Sphere{Vec3{center[0], center[1], center[2]}, 0.2, Material_Dielectric{1.5}});
                                }
                        }
                }
        }
	append(&world, Sphere{Vec3{0, 1, 0}, 1.0, Material_Dielectric{1.5}});
	append(&world, Sphere{Vec3{-4, 1, 0}, 1.0, Material_Lambertian{Vec3{0.4, 0.2, 0.1}}});
	append(&world, Sphere{Vec3{4, 1, 0}, 1.0, Material_Metal{Vec3{0.7, 0.6, 0.5}, 0.0}});
        return world;
}

main :: proc() {
	pcg.srand(0);
	nx := 200;
	ny := 100;
	ns := 1;
	fmt.printf("P3\n");
	fmt.printf("%d %d\n", nx, ny);
	fmt.printf("%d\n", 255);
	world := random_scene();
	lookfrom := Vec3{9, 2, 2.6};
	lookat := Vec3{3, .8, 1};
	dist_to_focus := vlen(lookfrom - lookat);
	aperture := f32(0.0);
	cam := make_camera(lookfrom, lookat, Vec3{0,1,0}, 30, f32(nx) / f32(ny), aperture, dist_to_focus);
when #defined(DEBUG) {
	cam_print(cam);
//	wprint(world[:]);
}
	for j := ny - 1; j >= 0; j -= 1 {
		for i := 0; i < nx; i += 1 {
			col := Vec3{0, 0, 0};
			for s := 0; s < ns; s += 1 {
when #defined(DEBUG) {
				fmt.printf("j=%d i=%d s=%d\n", j, i, s);
				fmt.printf("rfcnt=%d riuscnt=%d riudcnt=%d\n", rfcnt, riuscnt, riudcnt);
}
				u := (f32(i) + random_f()) / f32(nx);
				v := (f32(j) + random_f()) / f32(ny);
when #defined(DEBUG) {
//				fmt.printf("u=%.6f v=%.6f\n", u, v);
				uv := Vec3{u, v, 0};
				fmt.printf("uv=");vprint(uv);fmt.printf(" \n");
}
				r := get_ray(cam, u, v);
//				fmt.printf("j=%d i=%d s=%d r=", j, i, s);
//				rprint(r);
//				fmt.printf(" \n");
				col += color(world[:], r, 0);
			}
			col /= f32(ns);
			col = Vec3{math.sqrt(col[0]), math.sqrt(col[1]), math.sqrt(col[2])};
			ir := int(255.99 * col[0]);
			ig := int(255.99 * col[1]);
			ib := int(255.99 * col[2]);
			fmt.printf("%d %d %d\n", ir, ig, ib);
		}
	}
	delete(world);
}
