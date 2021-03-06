import math

type Vec3* = object
    x*, y*, z*: float32

{.push inline.}

func vec3*(x, y, z: float32): Vec3 =
    Vec3(x: x, y: y, z: z)

func squared_length*(v: Vec3): float32 =
    v.x * v.x + v.y * v.y + v.z * v.z

func length*(v: Vec3): float32 =
    sqrt(v.squared_length())

func vdot*(v1, v2: Vec3): float32 =
    v1.x * v2.x + v1.y * v2.y + v1.z * v2.z

func vcross*(v1: Vec3, v2: Vec3): Vec3 =
    result.x = v1.y * v2.z - v1.z * v2.y
    result.y = v1.z * v2.x - v1.x * v2.z
    result.z = v1.x * v2.y - v1.y * v2.x

func `-`*(v: Vec3): Vec3 =
    result.x = -v.x
    result.y = -v.y
    result.z = -v.z

func `/`*(v: Vec3, t: float32): Vec3 =
    result.x = v.x / t
    result.y = v.y / t
    result.z = v.z / t

func `*`*(t: float32, v: Vec3): Vec3 =
    result.x = t * v.x
    result.y = t * v.y
    result.z = t * v.z

func `+`*(v1: Vec3, v2: Vec3): Vec3 =
    result.x = v1.x + v2.x
    result.y = v1.y + v2.y
    result.z = v1.z + v2.z

func `-`*(v1: Vec3, v2: Vec3): Vec3 =
    result.x = v1.x - v2.x
    result.y = v1.y - v2.y
    result.z = v1.z - v2.z

func `*`*(v1: Vec3, v2: Vec3): Vec3 =
    result.x = v1.x * v2.x
    result.y = v1.y * v2.y
    result.z = v1.z * v2.z

func unit_vector*(v: Vec3): Vec3 =
    v / v.length()

func vreflect*(v: Vec3, n: Vec3): Vec3 =
    v - 2f * vdot(v, n) * n

{.pop.}
