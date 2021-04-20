module ray

import vec

pub struct Ray {
pub:
	origin vec.Vec3
	direction vec.Vec3
}

pub fn (r &Ray) str() string {
	return '{${r.origin.str()}, ${r.direction.str()}}'
}

[inline]
pub fn (r &Ray) point_at_parameter(t f32) vec.Vec3 {
	return r.origin + vec.mult(t, r.direction)
}
