module ray

import vec

pub struct Ray {
	a vec.Vec3
	b vec.Vec3
}

pub fn (r Ray) origin() vec.Vec3 {
	return r.a
}

pub fn (r Ray) direction() vec.Vec3 {
	return r.b
}

pub fn (r Ray) point_at_parameter(t f32) vec.Vec3 {
	return r.a + vec.mult(t, r.b)
}
