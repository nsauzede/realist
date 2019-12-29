module vec

import math

//[typedef]pub type Scalar f32
pub struct Vec3 {
pub:
//	x Scalar y Scalar z Scalar
	x f32 y f32 z f32
}
//pub fn (a Vec3) str() string {
//	return '{$a.x, $a.y, $a.z}'
//}

pub fn (a Vec3) +(b Vec3) Vec3 {
	return Vec3 {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z
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

pub fn (v Vec3) length() f32 {
	return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
}

pub fn (v Vec3) unit_vector() Vec3 {
	return div(v, v.length())
}

pub fn (a Vec3) dot(b Vec3) f32 {
	return a.x * b.x +
		a.y * b.y +
		a.z * b.z
}
