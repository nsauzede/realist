import strformat
import vec3
var nx = 200
var ny = 100
echo("P3")
echo(fmt"{nx} {ny}")
echo(255)
for j in countdown(ny - 1, 0):
 for i in countup(0, nx - 1):
  var col = vec3(i / nx, j / ny, 0.2)
  var ir = int(255.99 * col.x)
  var ig = int(255.99 * col.y)
  var ib = int(255.99 * col.z)
  echo(fmt"{ir} {ig} {ib}")
