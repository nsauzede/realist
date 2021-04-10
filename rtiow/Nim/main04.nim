import strformat
import vec3
import ray

func color(r:Ray): Vec3 =
 var unit_direction = unit_vector(r.direction)
 var t = 0.5 * (unit_direction.y + 1.0)
 result = (1.0 - t) * Vec3(x: 1.0, y: 1.0, z: 1.0) + t * Vec3(x: 0.5, y: 0.7, z: 1.0)

var nx = 200
var ny = 100
echo("P3")
echo(fmt"{nx} {ny}")
echo(255)
var lower_left_corner = Vec3(x: -2.0, y: -1.0, z: -1.0)
var horizontal = Vec3(x: 4.0, y: 0.0, z: 0.0)
var vertical = Vec3(x: 0.0, y: 2.0, z: 0.0)
var origin = Vec3(x: 0.0, y: 0.0, z: 0.0)
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
