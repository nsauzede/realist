import strformat, math
import vec3, ray, pcg

const FLT_MAX = 340282346638528859811704183484516925440.000000f

type Lambertian = object
    albedo: Vec3
type Metal = object
    albedo: Vec3
type MaterialKind = enum mkLambertian, mkMetal
type Material = ref object
    case kind: MaterialKind
    of mkLambertian: lambertian: Lambertian
    of mkMetal: metal: Metal

type HitRec = object
    t: float32
    p: Vec3
    normal: Vec3
    mat_ptr: Material

type HSphere = object
    center: Vec3
    radius: float32
    material: Material

proc random_in_unit_sphere(): Vec3 =
    while true:
        var r1 = random_f()
        var r2 = random_f()
        var r3 = random_f()
        result = 2f * vec3(r1, r2, r3) - vec3(1, 1, 1)
        if result.squared_length() < 1:
            break

func lambertian(albedo: Vec3): Material =
    Material(kind: mkLambertian, lambertian: Lambertian(albedo: albedo))
func metal(albedo: Vec3): Material =
    Material(kind: mkMetal, metal: Metal(albedo: albedo))
proc scatter(self: Lambertian, ray_in: Ray, rec: HitRec, attenuation: var Vec3,
        scattered: var Ray): bool =
    var target = rec.normal + random_in_unit_sphere()
    scattered = ray(rec.p, target)
    attenuation = self.albedo
    return true

proc scatter(self: Metal, ray_in: Ray, rec: HitRec, attenuation: var Vec3,
        scattered: var Ray): bool =
    var reflected = vreflect(unit_vector(ray_in.direction), rec.normal)
    scattered = ray(rec.p, reflected)
    attenuation = self.albedo
    return true

proc scatter(self: Material, ray_in: Ray, rec: HitRec, attenuation: var Vec3,
        scattered: var Ray): bool =
    case self.kind
    of mkLambertian:
        return self.lambertian.scatter(ray_in, rec, attenuation, scattered)
    of mkMetal:
        self.metal.scatter(ray_in, rec, attenuation, scattered)

type
    HittableKind = enum hkSphere
    Hittable = HittableObj
    HittableObj = object
        case kind: HittableKind
        of hkSphere: sphere: HSphere

type Camera = object
    origin: Vec3
    lower_left_corner: Vec3
    horizontal: Vec3
    vertical: Vec3

func get_ray(cam: Camera, u, v: float32): Ray =
    result = ray(cam.origin, cam.lower_left_corner + u * cam.horizontal +
                v * cam.vertical)

func hsphere(center: Vec3, radius: float32, material: Material): Hittable =
    Hittable(kind: hkSphere, sphere: HSphere(center: center, radius: radius,
            material: material))

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
            rec.mat_ptr = s.material
            return true
        temp = (-b + sqrt(discriminant)) / a
        if temp < t_max and temp > t_min:
            rec.t = temp
            rec.p = r.point_at_parameter(rec.t)
            rec.normal = (rec.p - s.center) / s.radius
            rec.mat_ptr = s.material
            return true
    return false

func hit(self: Hittable, r: Ray, t_min, t_max: float32,
    rec: var HitRec): bool =
    case self.kind
    of hkSphere:
        return hit_sphere(self.sphere, r, t_min, t_max, rec)

func hit(hh: openArray[Hittable], r: Ray, t_min, t_max: float32,
    rec: var HitRec): bool =
    var temp_rec: HitRec
    var hit_anything = false
    var closest_so_far = t_max
    for h in hh:
        if h.hit(r, t_min, closest_so_far, temp_rec):
            hit_anything = true
            closest_so_far = temp_rec.t
            rec = temp_rec
    return hit_anything

proc color(r: Ray, world: openArray[Hittable], depth: int32): Vec3 =
    var rec: HitRec
    if hit(world, r, 0.001, FLT_MAX, rec):
        var scattered: Ray
        var attenuation: Vec3
        if depth < 50 and scatter(rec.mat_ptr, r, rec, attenuation, scattered):
            return attenuation * color(scattered, world, depth + 1)
        else:
            return vec3(0, 0, 0)
    var unit_direction = unit_vector(r.direction)
    var t = 0.5f * (unit_direction.y + 1f)
    result = (1f - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1)

pcg.srand(0)
var (nx, ny, ns) = (200, 100, 100)
echo(&"P3\n{nx} {ny}\n255")
var world: seq[Hittable] = @[
    hsphere(vec3(0, 0, -1), 0.5, lambertian(vec3(0.8, 0.3, 0.3))),
    hsphere(vec3(0, -100.5, -1), 100, lambertian(vec3(0.8, 0.8, 0.0))),
    hsphere(vec3(1, 0, -1), 0.5, metal(vec3(0.8, 0.6, 0.2))),
    hsphere(vec3(-1, 0, -1), 0.5, metal(vec3(0.8, 0.8, 0.8))),
]
var cam = Camera(
    lower_left_corner: vec3(-2, -1, -1),
    horizontal: vec3(4, 0, 0),
    vertical: vec3(0, 2, 0),
    origin: vec3(0, 0, 0),
)
for j in countdown(ny - 1, 0):
    for i in countup(0, nx - 1):
        var col = vec3(0, 0, 0)
        # stdout.write(&"j={j} i={i}\n")
        for s in countup(0, ns-1):
            var u = (float32(i) + random_f()) / float32(nx)
            var v = (float32(j) + random_f()) / float32(ny)
            var r = cam.get_ray(u, v)
            # stdout.write(&"u={u:.6f} v={v:.6f}\n")
            col = col + color(r, world, 0)
            # stdout.write(&"col={col}\n")
            # stdout.write(&"col=(x: {col.x:.16f}, y: {col.y:.16f}, z: {col.z:.16f})\n")
        col = col / float32(ns)
        col = vec3(sqrt(col.x), sqrt(col.y), sqrt(col.z))
        # stdout.write(&"col=(x: {col.x:.16f}, y: {col.y:.16f}, z: {col.z:.16f})\n")
        # stdout.write(&"{{{col.x:.6f}, {col.y:.6f}, {col.z:.6f};{cast[uint32](col.x):x}, {cast[uint32](col.y):x}, {cast[uint32](col.z):x}}}")
        var ir = int32(255.99f * col.x)
        var ig = int32(255.99f * col.y)
        var ib = int32(255.99f * col.z)
        stdout.write(&"{ir} {ig} {ib}\n")
