import strformat
import math
import vec3
import ray

func hit_sphere(center: Vec3, radius: float32, r: Ray): float32 =
 var oc = r.origin - center
 var a = dot(r.direction, r.direction)
 var b = 2 * dot(oc, r.direction)
 var c = dot(oc, oc) - radius * radius
 var discriminant = b*b - 4*a*c
 if discriminant < 0:
  return -1
 else:
  return (-b - sqrt(discriminant)) / (2*a)

func color(r: Ray): Vec3 =
 var t = hit_sphere(vec3(0, 0, -1), 0.5, r)
 if t > 0:
  var N = unit_vector(r.point_at_parameter(t) - vec3(0, 0, -1))
  return 0.5*vec3(N.x+1, N.y+1, N.z+1)
 var unit_direction = unit_vector(r.direction)
 t = 0.5 * (unit_direction.y + 1)
 result = (1 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1)

var (nx, ny) = (200, 100)
echo("P3")
echo(fmt"{nx} {ny}")
echo(255)
var lower_left_corner = vec3(-2, -1, -1)
var horizontal = vec3(4, 0, 0)
var vertical = vec3(0, 2, 0)
var origin = vec3(0, 0, 0)
for j in countdown(ny - 1, 0):
 for i in countup(0, nx - 1):
  var u = float32(i) / float32(nx)
  var v = float32(j) / float32(ny)
  var r = ray(origin, lower_left_corner + u*horizontal + v*vertical)
  var col = color(r)
  var ir = int(255.99 * col.x)
  var ig = int(255.99 * col.y)
  var ib = int(255.99 * col.z)
  echo(fmt"{ir} {ig} {ib}")
