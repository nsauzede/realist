import vec3
type Ray* = object
    origin*: Vec3
    direction*: Vec3

{.push inline.}

func ray*(origin: Vec3, direction: Vec3): Ray =
    result.origin = origin
    result.direction = direction

func point_at_parameter*(r: Ray, t: float32): Vec3 =
    r.origin + t * r.direction

{.pop.}
