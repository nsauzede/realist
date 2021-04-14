package main
// #cgo LDFLAGS:  -lm
// #include <math.h>
import "C"
import (
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

type Camera struct {
	lower_left_corner Vec3
	horizontal Vec3
	vertical Vec3
	origin Vec3
}

func (c Camera) get_ray(u float32, v float32) Ray {
	return Ray{c.origin,
		vsub(vadd(c.lower_left_corner, vadd(vmul(u,c.horizontal), vmul(v,c.vertical))), c.origin)}
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

func vsub(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]}
}

func vdot(v1 Vec3, v2 Vec3) float32 {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]
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

func main() {
	pcg.Srand(0)
	nx := 200
	ny := 100
	ns := 100
	fmt.Printf("P3\n")
	fmt.Printf("%d %d\n", nx, ny)
	fmt.Printf("%d\n", 255)
	world := []Hittable{
		Sphere{Vec3{0, 0, -1}, 0.5, Lambertian{Vec3{0.1, 0.2, 0.5}}},
		Sphere{Vec3{0, -100.5, -1}, 100, Lambertian{Vec3{0.8, 0.8, 0.}}},
		Sphere{Vec3{1., 0., -1.}, 0.5, Metal{Vec3{0.8, 0.6, 0.2}, 0.3}},
		Sphere{Vec3{-1., 0., -1.}, 0.5, Dielectric{1.5}},
		Sphere{Vec3{-1., 0., -1.}, -0.45, Dielectric{1.5}},
	}
//	fmt.Print("world=",world,"\n")
	cam := Camera {
		lower_left_corner: Vec3{-2.,-1.,-1.},
		horizontal: Vec3{4.,0.,0.},
		vertical: Vec3{0.,2.,0.},
		origin: Vec3{0.,0.,0.},
	}
//	fmt.Print("cam=",cam,"\n")
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			col := Vec3{0, 0, 0}
			for s := 0; s < ns; s++ {
				u := (float32(i) + random_double()) / float32(nx)
				v := (float32(j) + random_double()) / float32(ny)
//				uv := Vec3{u, v, 0}
//				fmt.Print("uv=");uv.print();fmt.Printf(" \n")
				r := cam.get_ray(u, v)
				col0 := color(r, world, 0)
				col = vadd(col, col0)
			}
			col = vdiv(col, float32(ns))
			col = Vec3{Sqrtf(col[0]), Sqrtf(col[1]), Sqrtf(col[2])};
			ir := int(255.99 * col[0])
			ig := int(255.99 * col[1])
			ib := int(255.99 * col[2])
			fmt.Printf("%d %d %d\n", ir, ig, ib)
		}
	}
}
