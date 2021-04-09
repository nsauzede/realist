module ray

import vec

pub struct Ray {
	a vec.Vec3
	b vec.Vec3
}

pub fn (r Ray) str() string {
	return '{${r.a.str()}, ${r.b.str()}}'
}

[inline]
pub fn (r Ray) origin() vec.Vec3 {
	return r.a
}

[inline]
pub fn (r Ray) direction() vec.Vec3 {
	return r.b
}

[inline]
pub fn (r Ray) point_at_parameter(t f32) vec.Vec3 {
	return r.a + vec.mult(t, r.b)
}
