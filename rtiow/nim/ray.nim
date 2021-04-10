import vec3
type
 Ray* = object
  origin*: Vec3
  direction*: Vec3

func ray*(origin: Vec3, direction: Vec3): Ray {.inline.} =
 result.origin = origin
 result.direction = direction
