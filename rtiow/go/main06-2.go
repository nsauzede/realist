package main

import ("fmt"; "math"; "C")

type Vec3 [3]float32

type Ray struct {
	origin Vec3
	direction Vec3
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

func vsub(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]}
}

func vdot(v1 Vec3, v2 Vec3) float32 {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]
}

// #include <math.h>
func Sqrtf(x float32) float32 {
	return float32(math.Sqrt(float64(x)))
}
func (v Vec3) len() float32 {
//	return math.Sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])
	return Sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])
//	return C.sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])
}

func (v Vec3) unit_vector() Vec3 {
	return vdiv(v, v.len())
}

func (r Ray) point_at_parameter(t float32) Vec3 {
	return vadd(r.origin, vmul(t, r.direction))
}

func hit_sphere(center Vec3, radius float32, r Ray) float32 {
	oc := vsub(r.origin, center)
	a := vdot(r.direction, r.direction)
	b := 2.0 * vdot(oc, r.direction)
	c := vdot(oc, oc) - radius * radius
	discriminant := b * b - 4 * a * c
	if discriminant < 0 {
		return -1.0
	} else {
		return (-b - Sqrtf(discriminant)) / (2.0 * a)
	}
}

type HSphere struct {
	center Vec3
	radius float32
}

type HitRec struct {
	t float32
	p Vec3
	normal Vec3
}

func (s HSphere) hit(r Ray, t_min float32, t_max float32, rec *HitRec) bool {
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
			return true
		}
		temp = (-b + Sqrtf(discriminant)) / a
		if temp < t_max && temp > t_min {
			rec.t = temp
			rec.p = r.point_at_parameter(rec.t)
			rec.normal = vdiv(vsub(rec.p, s.center), s.radius)
			return true
		}
	}
	return false
}

type HSphereS []HSphere

func (hh HSphereS) hit(r Ray, t_min float32, t_max float32, rec *HitRec) bool {
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

func color(r Ray, world HSphereS) Vec3 {
	rec := HitRec{}
	if world.hit(r, 0., 99999., &rec) {
		return vmul(0.5, vadd(rec.normal, Vec3{1, 1, 1}))
	}
	unit_direction := r.direction.unit_vector()
	t := 0.5 * (unit_direction[1] + 1.0)
	return vadd(vmul(1.0 - t, Vec3{1.0, 1.0, 1.0}), vmul(t, Vec3{0.5, 0.7, 1.0}))
}

func main() {
	nx := 200
	ny := 100
	fmt.Printf("P3\n")
	fmt.Printf("%d %d\n", nx, ny)
	fmt.Printf("%d\n", 255)
	lower_left_corner := Vec3{-2.0, -1.0, -1.0}
	horizontal := Vec3{4.0, 0.0, 0.0}
	vertical := Vec3{0.0, 2.0, 0.0}
	origin := Vec3{0.0, 0.0, 0.0}
	world := []HSphere{
		HSphere{Vec3{0, 0, -1}, 0.5},
		HSphere{Vec3{0, -100.5, -1}, 100},
	}
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			u := float32(i) / float32(nx)
			v := float32(j) / float32(ny)
			r := Ray{origin, vadd(lower_left_corner, vadd(vmul(u, horizontal), vmul(v, vertical)))}
			col := color(r, world)
			ir := int(255.99 * col[0])
			ig := int(255.99 * col[1])
			ib := int(255.99 * col[2])
			fmt.Printf("%d %d %d\n", ir, ig, ib)
		}
	}
}
