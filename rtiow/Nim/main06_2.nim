import strformat
import math
import vec3
import ray

type HitRec = object
  t: float32
  p: Vec3
  normal: Vec3

type HSphere = object
  center: Vec3
  radius: float32

func hsphere(center: Vec3, radius: float32): HSphere =
  HSphere(center: center, radius: radius)

func hit_sphere(s: HSphere, r: Ray, t_min, t_max: float32,
    rec: var HitRec): bool =
  var oc = r.origin - s.center
  var a = dot(r.direction, r.direction)
  var b = dot(oc, r.direction)
  var c = dot(oc, oc) - s.radius * s.radius
  var discriminant = b*b - a*c
  if discriminant > 0:
    var temp = (-b - sqrt(discriminant)) / a
    if temp < t_max and temp > t_min:
      rec.t = temp
      rec.p = r.point_at_parameter(rec.t)
      rec.normal = (rec.p - s.center) / s.radius
      return true
    temp = (-b + sqrt(discriminant)) / a
    if temp < t_max and temp > t_min:
      rec.t = temp
      rec.p = r.point_at_parameter(rec.t)
      rec.normal = (rec.p - s.center) / s.radius
      return true
  return false

func hit(hh: openArray[HSphere], r: Ray, t_min, t_max: float32,
    rec: var HitRec): bool =
  var temp_rec: HitRec
  var hit_anything = false
  var closest_so_far = t_max
  for h in hh:
    if hit_sphere(h, r, t_min, closest_so_far, temp_rec):
      hit_anything = true
      closest_so_far = temp_rec.t
      rec = temp_rec
  return hit_anything

func color(r: Ray, world: openArray[HSphere]): Vec3 =
  var rec: HitRec
  if hit(world, r, 0, 99999, rec):
    return 0.5*(rec.normal + vec3(1, 1, 1))
  var unit_direction = unit_vector(r.direction)
  var t = 0.5 * (unit_direction.y + 1)
  result = (1 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1)

var (nx, ny) = (200, 100)
echo(&"P3\n{nx} {ny}\n255")
var lower_left_corner = vec3(-2, -1, -1)
var horizontal = vec3(4, 0, 0)
var vertical = vec3(0, 2, 0)
var origin = vec3(0, 0, 0)
var world: seq[HSphere] = @[
  hsphere(vec3(0, 0, -1), 0.5),
  hsphere(vec3(0, -100.5, -1), 100)
  ]
for j in countdown(ny - 1, 0):
  for i in countup(0, nx - 1):
    var u = float32(i) / float32(nx)
    var v = float32(j) / float32(ny)
    var r = ray(origin, lower_left_corner + u*horizontal + v*vertical)
    var col = color(r, world)
    var ir = int(255.99 * col.x)
    var ig = int(255.99 * col.y)
    var ib = int(255.99 * col.z)
    stdout.write(&"{ir} {ig} {ib}\n")
