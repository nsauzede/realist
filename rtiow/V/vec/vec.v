module vec

import math

//[typedef]pub type Scalar f32
pub struct Vec3 {
pub:
//	x Scalar y Scalar z Scalar
	x f32 y f32 z f32
}

union U0 {
	v [3]f32
	i [3]u32
}

struct S0 {
	u U0
}

pub fn (a Vec3) str() string {
//	return '{$a.x, $a.y, $a.z}'
	mut u0 := U0{}
	u0.v[0] = a.x
	u0.v[1] = a.y
	u0.v[2] = a.z
	p1 := u0.i[0].hex()
	p2 := u0.i[1].hex()
	p3 := u0.i[2].hex()
	return '{$a.x, $a.y, $a.z;$p1, $p2, $p3}'
}

pub fn (a Vec3) +(b Vec3) Vec3 {
	return Vec3 {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z
	}
}

pub fn (a Vec3) *(b Vec3) Vec3 {
	return Vec3 {
		a.x * b.x,
		a.y * b.y,
		a.z * b.z
	}
}

pub fn (v Vec3) cross(ov Vec3) Vec3 {
	return Vec3 {
		v.y * ov.z - v.z * ov.y,
		v.z * ov.x - v.x * ov.z,
		v.x * ov.y - v.y * ov.x
	}
}

pub fn (a Vec3) -(b Vec3) Vec3 {
	return Vec3 {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z
	}
}

pub fn mult(k f32, v Vec3) Vec3 {
	return Vec3 {
		k * v.x,
		k * v.y,
		k * v.z
	}
}

pub fn div(v Vec3, k f32) Vec3 {
	return Vec3 {
		v.x / k,
		v.y / k,
		v.z / k
	}
}

pub fn (v Vec3) reflect(n Vec3) Vec3 {
	return v - mult(2. * v.dot(n), n)
}

pub fn (v Vec3) refract(n Vec3, ni_over_nt f32, refracted mut Vec3) bool {
	uv := v.unit_vector()
	dt := uv.dot(n)
	discriminant := 1. - ni_over_nt * ni_over_nt * (1. - dt * dt)
	if discriminant > 0 {
		*refracted = mult(ni_over_nt, uv - mult(dt, n)) - mult(math.sqrtf(discriminant), n)
		return true
	} else {
		return false
	}
}

pub fn (v Vec3) length() f32 {
	return math.sqrtf(v.x * v.x + v.y * v.y + v.z * v.z)
}

pub fn (v Vec3) squared_length() f32 {
	return v.x * v.x + v.y * v.y + v.z * v.z
}

pub fn (v Vec3) unit_vector() Vec3 {
	return div(v, v.length())
}

pub fn (a Vec3) dot(b Vec3) f32 {
	return a.x * b.x +
		a.y * b.y +
		a.z * b.z
/*
	print('DOT $a.x,$a.y,$a.z;$b.x,$b.y,$b.z')
	ret := a.x * b.x +
		a.y * b.y +
		a.z * b.z
	println(' => $ret')
	return ret
*/
}
