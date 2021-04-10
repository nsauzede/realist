import strformat
var nx = 200
var ny = 100
echo("P3")
echo(fmt"{nx} {ny}")
echo(255)
for j in countdown(ny - 1, 0):
 for i in countup(0, nx - 1):
  var r = i / nx
  var g = j / ny
  var b = 0.2
  var ir = int(255.99 * r)
  var ig = int(255.99 * g)
  var ib = int(255.99 * b)
  echo(fmt"{ir} {ig} {ib}")
