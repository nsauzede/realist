package main
// #cgo LDFLAGS:  -lm
// #include <math.h>
import "C"
import (
	"os"
	"bufio"
	"fmt"
	"math"
	"./src/pcg"
)

type Vec3 [3]float32

type Ray struct {
	origin Vec3
	direction Vec3
}

type Hittable interface {
	hit(r Ray, t_min float32, t_max float32, rec *HitRec) bool
}

type Material interface {
	scatter(r Ray, rec HitRec, attenuation *Vec3, scattered *Ray) bool
}

type Lambertian struct {
	albedo Vec3
}

type Metal struct {
	albedo Vec3
	fuzz float32
}

type Dielectric struct {
	ref_idx float32
}

type Sphere struct {
	center Vec3
	radius float32
	material Material
}

type HitRec struct {
	t float32
	p Vec3
	normal Vec3
	mat Material
}

type Camera struct {
	lower_left_corner Vec3
	horizontal Vec3
	vertical Vec3
	origin Vec3
	u Vec3
	v Vec3
	w Vec3
	lens_radius float32
}

func (m Lambertian) scatter(r Ray, rec HitRec, attenuation *Vec3, scattered *Ray) bool {
	target := vadd(rec.normal, random_in_unit_sphere())
	*scattered = Ray{rec.p, target}
	*attenuation = m.albedo
	return true
}

func (m Metal) scatter(ray_in Ray, rec HitRec, attenuation *Vec3, scattered *Ray) bool {
	reflected := ray_in.direction.unit_vector().reflect(rec.normal)
	*scattered = Ray{rec.p, vadd(reflected, vmul(m.fuzz, random_in_unit_sphere()))}
	*attenuation = m.albedo
	return vdot(scattered.direction, rec.normal) > 0
}

func (v Vec3) refract(n Vec3, ni_over_nt float32, refracted *Vec3) bool {
	uv := v.unit_vector()
	dt := vdot(uv, n)
	discriminant := 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt)
	if discriminant > 0 {
		*refracted = vsub(vmul(ni_over_nt, vsub(uv, vmul(dt, n))), vmul(Sqrtf(discriminant), n))
		return true
	} else {
		return false
	}
}

func schlick(cosine float32, ref_idx float32) float32 {
	r0 := (1.0 - ref_idx) / (1.0 + ref_idx)
	r0 = r0 * r0
	return r0 + (1.0 - r0) * Powf(1.0 - cosine, 5.0)
}

func (m Dielectric) scatter(ray_in Ray, rec HitRec, attenuation *Vec3, scattered *Ray) bool {
	outward_normal := Vec3{}
	reflected := ray_in.direction.reflect(rec.normal)
	ni_over_nt := float32(0.0)
	*attenuation = Vec3{1, 1, 1}
	refracted := Vec3{}
	reflect_prob := float32(0.0)
	cosine := float32(0.0)
	if vdot(ray_in.direction, rec.normal) > 0 {
		outward_normal = vmul(-1, rec.normal)
		ni_over_nt = m.ref_idx
		cosine = m.ref_idx * vdot(ray_in.direction, rec.normal) / ray_in.direction.len()
	} else {
		outward_normal = rec.normal
		ni_over_nt = 1.0 / m.ref_idx
		cosine = -1.0 * vdot(ray_in.direction, rec.normal) / ray_in.direction.len()
	}
	if ray_in.direction.refract(outward_normal, ni_over_nt, &refracted) {
		reflect_prob = schlick(cosine, m.ref_idx)
	} else {
		reflect_prob = 1.0
	}
	if random_double() < reflect_prob {
		*scattered = Ray{rec.p, reflected}
	} else {
		*scattered = Ray{rec.p, refracted}
	}
	return true
}

func random_double() float32 {
	return float32(pcg.Rand()) / (float32(pcg.RAND_MAX) + float32(1.0))
}

func random_in_unit_sphere() Vec3 {
	p := Vec3{}
	for {
		r1 := random_double()
		r2 := random_double()
		r3 := random_double()
		p = vsub(vmul(2., Vec3{r1, r2, r3}), Vec3{1, 1, 1})
		if p.squared_length() < 1.0 {
			break
		}
	}
	return p
}

func random_in_unit_disk() Vec3 {
	p := Vec3{};
	for {
		r1 := random_double()
		r2 := random_double()
		p = vsub(vmul(2.0, Vec3{r1, r2, 0}), Vec3{1,1,0})
		if p.squared_length() < 1.0 {
			break
		}
	}
	return p
}

func vmul(f float32, v Vec3) Vec3 {
	return Vec3{f * v[0], f * v[1], f * v[2]}
}

func vdiv(v Vec3, f float32) Vec3 {
	return Vec3{v[0] / f, v[1] / f, v[2] / f}
}

func vadd(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]}
}

func vmulv(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]}
}

func vcross(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{
		v1[1] * v2[2] - v1[2] * v2[1],
		v1[2] * v2[0] - v1[0] * v2[2],
		v1[0] * v2[1] - v1[1] * v2[0],
	}
}

func vsub(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]}
}

func vdot(v1 Vec3, v2 Vec3) float32 {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]
}

func Cosf(x float32) float32 {
	return float32(math.Cos(float64(x)))
}

func Tanf(x float32) float32 {
	return float32(math.Tan(float64(x)))
}

func Sqrtf(x float32) float32 {
	return float32(math.Sqrt(float64(x)))
}

func Powf(x float32, y float32) float32 {
	return float32(math.Pow(float64(x), float64(y)))
}

func (v Vec3) len() float32 {
	return Sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])
}

func (v Vec3) squared_length() float32 {
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]
}

func (v Vec3) unit_vector() Vec3 {
	return vdiv(v, v.len())
}

func (v Vec3) reflect(n Vec3) Vec3 {
	return vsub(v, vmul(2.0 * vdot(v, n), n))
}

func (r Ray) point_at_parameter(t float32) Vec3 {
	return vadd(r.origin, vmul(t, r.direction))
}

func (s Sphere) hit(r Ray, t_min float32, t_max float32, rec *HitRec) bool {
	oc := vsub(r.origin, s.center)
	a := vdot(r.direction, r.direction)
	b := vdot(oc, r.direction)
	c := vdot(oc, oc) - s.radius * s.radius
	discriminant := b * b - a * c
	if discriminant > 0. {
		temp := (-b - Sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vdiv(vsub(rec.p, s.center), s.radius)
			rec.mat = s.material
			return true
		}
		temp = (-b + Sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vdiv(vsub(rec.p, s.center), s.radius)
			rec.mat = s.material
			return true
		}
	}
	return false
}

type HittableS []Hittable

func (hh HittableS) hit(r Ray, t_min float32, t_max float32, rec *HitRec) bool {
	temp_rec := HitRec{}
	hit_anything := false
	closest_so_far := t_max
	for _, h := range hh {
		if h.hit(r, t_min, closest_so_far, &temp_rec) {
			hit_anything = true
			closest_so_far = temp_rec.t
			*rec = temp_rec
		}
	}
	return hit_anything
}

func color(r Ray, world HittableS, depth int) Vec3 {
	rec := HitRec{}
	if world.hit(r, 0.001, math.MaxFloat32, &rec) {
		scattered := Ray{}
		attenuation := Vec3{}
		if depth < 50 && rec.mat.scatter(r, rec, &attenuation, &scattered) {
			return vmulv(attenuation, color(scattered, world, depth + 1))
		} else {
			return Vec3{0, 0, 0}
		}
	} else {
		unit_direction := r.direction.unit_vector()
		t := 0.5 * (unit_direction[1] + 1.0)
		return vadd(vmul(1.0 - t, Vec3{1.0, 1.0, 1.0}), vmul(t, Vec3{0.5, 0.7, 1.0}))
	}
}

func (v Vec3) print() {
	fmt.Printf("{%f, %f, %f;%x, %x, %x}", v[0], v[1], v[2], math.Float32bits(v[0]), math.Float32bits(v[1]), math.Float32bits(v[2]))
}

func (r Ray) print() {
	fmt.Printf("{")
	r.origin.print()
	fmt.Printf(", ");
	r.direction.print()
	fmt.Printf("} ");
}

func vprint(v Vec3) {
	v.print()
}

func cam_print(cam Camera) {
	fmt.Printf("{\n\tlower_left_corner: ");vprint(cam.lower_left_corner);fmt.Printf(" ");
	fmt.Printf("\n\thorizontal: ");vprint(cam.horizontal);fmt.Printf(" ");
	fmt.Printf("\n\tvertical: ");vprint(cam.vertical);fmt.Printf(" ");
	fmt.Printf("\n\torigin: ");vprint(cam.origin);fmt.Printf(" ");
	fmt.Printf("\n}");
	fmt.Printf("\nu: ");vprint(cam.u);
	fmt.Printf("\nv: ");vprint(cam.v);
	fmt.Printf("\nw: ");vprint(cam.w);
	fmt.Printf("\nlens_radius=%.6f\n\n", cam.lens_radius);
}

func make_camera(lookfrom Vec3, lookat Vec3, vup Vec3, vfov float32, aspect float32, aperture float32, focus_dist float32) Camera {
	cam := Camera{}
	theta := vfov * float32(math.Pi) / float32(180)
	half_height := Tanf(theta / 2)
	half_width := aspect * half_height
//	thw := Vec3{theta, half_height, half_width}
//	fmt.Print("thw=");thw.print();fmt.Printf(" \n")
	cam.w = vsub(lookfrom, lookat).unit_vector()
	cam.u = vcross(vup, cam.w).unit_vector()
	cam.v = vcross(cam.w, cam.u)
	cam.origin = lookfrom
/*
cam.lower_left_corner = 
vsub(
 lookfrom, 
 vadd(
 vmul(half_width * focus_dist, cam.u), 
 vadd(
  vmul(half_height * focus_dist, cam.v), 
  vmul(focus_dist, cam.w))))
*/
	tmp := vmul(half_width * focus_dist, cam.u)
	cam.lower_left_corner = vsub(cam.origin, tmp)
	tmp = vmul(half_height * focus_dist, cam.v)
	cam.lower_left_corner = vsub(cam.lower_left_corner, tmp)
	tmp = vmul(focus_dist, cam.w)
	cam.lower_left_corner = vsub(cam.lower_left_corner, tmp)

	cam.horizontal = vmul(2 * half_width * focus_dist, cam.u)
	cam.vertical = vmul(2 * half_height * focus_dist, cam.v)
	cam.lens_radius = aperture / 2
	return cam
}

func (c Camera) get_ray(s float32, t float32) Ray {
	ray := Ray{}
	rd := vmul(c.lens_radius, random_in_unit_disk())
//	offset := vadd(vmul(rd[0], c.u), vmul(rd[1], c.v))
//	return Ray{vadd(c.origin, offset),
//vsub(vadd(c.lower_left_corner, vadd(vmul(s,c.horizontal), vmul(t,c.vertical))),vadd(c.origin,offset))}
	offset := vmul(rd[0], c.u)
	tmp := vmul(rd[1], c.v)
	offset = vadd(offset, tmp)
	tmp = vmul(s, c.horizontal)
	ray.direction = vadd(c.lower_left_corner, tmp)
	tmp = vmul(t, c.vertical)
	ray.direction = vadd(ray.direction, tmp)
	ray.direction = vsub(ray.direction, c.origin)
	ray.direction = vsub(ray.direction, offset)
	ray.origin = vadd(c.origin, offset)
	return ray
}

func random_scene() []Hittable {
	world := []Hittable {
		Sphere{Vec3{0, -1000, 0}, 1000, Lambertian{Vec3{0.5, 0.5, 0.5}}},
	}
	for a:=-11; a < 11; a += 1 {
		for b:=-11; b < 11; b += 1 {
			choose_mat := random_double()
			r1 := random_double()
			r2 := random_double()
			center := Vec3{float32(a) + 0.9 * r1, 0.2, float32(b) + 0.9 * r2}
			if vsub(center, Vec3{4, 0.2, 0}).len() > 0.9 {
				if (choose_mat < 0.8) { // diffuse
					r1 := random_double()
					r2 := random_double()
					r3 := random_double()
					r4 := random_double()
					r5 := random_double()
					r6 := random_double()
world = append(world, Sphere{center, 0.2, Lambertian{Vec3{r1 * r2, r3 * r4, r5 * r6}}})
				} else if choose_mat < 0.95 { // metal
					r1 := 0.5 * (1. + random_double())
					r2 := 0.5 * (1. + random_double())
					r3 := 0.5 * (1. + random_double())
					r4 := 0.5 * random_double()
world = append(world, Sphere{center, 0.2, Metal{Vec3{r1, r2, r3}, r4}})
				} else { // glass
world = append(world, Sphere{center, 0.2, Dielectric{1.5}})
				}
			}
		}
	}
world = append(world, Sphere{Vec3{0, 1, 0}, 1, Dielectric{1.5}})
world = append(world, Sphere{Vec3{-4, 1, 0}, 1, Lambertian{Vec3{0.4, 0.2, 0.1}}})
world = append(world, Sphere{Vec3{4, 1, 0}, 1, Metal{Vec3{0.7, 0.6, 0.5}, 0.}})
	return world
}

func main() {
	pcg.Srand(0)
	fnameout := ""
	fout := os.Stdout
	var err error
	var bytes []byte
	var nbytes = 0
	nx := 200
	ny := 100
	ns := 1
	argv := os.Args
	arg, argc := 0, len(argv)
	if arg < argc - 1 {
		arg++
		fmt.Sscanf(argv[arg], "%d", &nx)
		if arg < argc {
			arg++
			fmt.Sscanf(argv[arg], "%d", &ny)
			if arg < argc {
				arg++
				fmt.Sscanf(argv[arg], "%d", &ns)
				if arg < argc {
					arg++
					fnameout = argv[arg]
				}
			}
		}
	}
	if fnameout != "" {
		fout,err = os.Create( fnameout)
		if err != nil {
			panic(err)
		}
		defer fout.Close()
	}
	nbytes = 3 * nx * ny
	bytes = make([]byte, nbytes)
	buf := bufio.NewWriter(fout)
	fmt.Fprintf(buf,"P6\n")
	fmt.Fprintf(buf,"%d %d\n", nx, ny)
	fmt.Fprintf(buf,"%d\n", 255)
	world := random_scene()
	lookfrom := Vec3{9, 2, 2.6}
	lookat := Vec3{3, .8, 1}
	dist_to_focus := vsub(lookfrom, lookat).len()
	aperture := float32(0)
	cam := make_camera(lookfrom, lookat, Vec3{0,1,0}, 30, float32(nx) / float32(ny), aperture, dist_to_focus);
//	cam_print(cam)
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			col := Vec3{0, 0, 0}
			for s := 0; s < ns; s++ {
				u := (float32(i) + random_double()) / float32(nx)
				v := (float32(j) + random_double()) / float32(ny)
//				uv := Vec3{u, v, 0}
//				fmt.Print("uv=");uv.print();fmt.Printf(" \n")
				r := cam.get_ray(u, v)
//				fmt.Print("r=");r.print();fmt.Printf("\n")
				col0 := color(r, world, 0)
				col = vadd(col, col0)
			}
			col = vdiv(col, float32(ns))
			col = Vec3{Sqrtf(col[0]), Sqrtf(col[1]), Sqrtf(col[2])};
			ir := int(255.99 * col[0])
			ig := int(255.99 * col[1])
			ib := int(255.99 * col[2])
			bytes[((ny - 1 - j) * nx + i) * 3 + 0] = byte(ir)
			bytes[((ny - 1 - j) * nx + i) * 3 + 1] = byte(ig)
			bytes[((ny - 1 - j) * nx + i) * 3 + 2] = byte(ib)
		}
	}
	buf.Write(bytes)
//	buf.Flush()
}
