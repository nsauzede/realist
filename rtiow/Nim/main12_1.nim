import strformat, math
import vec3, ray, pcg

const FLT_MAX = 340282346638528859811704183484516925440.000000f

type Lambertian = object
    albedo: Vec3
type Metal = object
    albedo: Vec3
    fuzz: float32
type Dielectric = object
    ref_idx: float32
type MaterialKind = enum mkLambertian, mkMetal, mkDielectric
type Material = ref object
    case kind: MaterialKind
    of mkLambertian: lambertian: Lambertian
    of mkMetal: metal: Metal
    of mkDielectric: dielectric: Dielectric

type HSphere = object
    center: Vec3
    radius: float32
    material: Material

type
    HittableKind = enum hkSphere
    Hittable = HittableObj
    HittableObj = object
        case kind: HittableKind
        of hkSphere: sphere: HSphere
    HitRec = object
        t: float32
        p: Vec3
        normal: Vec3
        mat_ptr: Material

var n_rand: uint32 = 0

proc random_in_unit_sphere(): Vec3 =
    while true:
        var r1 = random_f()
        var r2 = random_f()
        var r3 = random_f()
        n_rand+=3
        result = 2f * vec3(r1, r2, r3) - vec3(1, 1, 1)
        if result.squared_length() < 1f:
            break

proc random_in_unit_disk(): Vec3 =
    while true:
        var r1 = random_f()
        var r2 = random_f()
        n_rand+=2
        result = 2f * vec3(r1, r2, 0) - vec3(1, 1, 0)
        if result.squared_length() < 1f:
            break

func lambertian(albedo: Vec3): Material =
    Material(kind: mkLambertian, lambertian: Lambertian(albedo: albedo))
func metal(albedo: Vec3, fuzz: float32): Material =
    Material(kind: mkMetal, metal: Metal(albedo: albedo, fuzz: fuzz))
func dielectric(ref_idx: float32): Material =
    Material(kind: mkDielectric, dielectric: Dielectric(ref_idx: ref_idx))

proc scatter(self: Lambertian, ray_in: Ray, rec: HitRec, attenuation: var Vec3,
        scattered: var Ray): bool =
    var target = rec.normal + random_in_unit_sphere()
    scattered = ray(rec.p, target)
    attenuation = self.albedo
    return true

proc scatter(self: Metal, ray_in: Ray, rec: HitRec, attenuation: var Vec3,
        scattered: var Ray): bool =
    var reflected = vreflect(unit_vector(ray_in.direction), rec.normal)
    scattered = ray(rec.p, reflected + self.fuzz * random_in_unit_sphere())
    attenuation = self.albedo
    return vdot(scattered.direction, rec.normal) > 0f

func refract(v: Vec3, n: Vec3, ni_over_nt: float32, refracted: var Vec3): bool =
    var uv = unit_vector(v)
    var dt = vdot(uv, n)
    var discriminant = 1f - ni_over_nt * ni_over_nt * (1f - dt * dt)
    if discriminant > 0f:
        refracted = ni_over_nt * (uv - dt * n) - sqrt(discriminant) * n
        return true
    else:
        return false

func schlick(cosine: float32, ref_idx: float32): float32 =
    var r0 = (1f - ref_idx) / (1f + ref_idx)
    r0 = r0 * r0
    return r0 + (1f - r0) * pow(1f - cosine, 5f)

proc scatter(self: Dielectric, ray_in: Ray, rec: HitRec, attenuation: var Vec3,
    scattered: var Ray): bool =
    var reflected = vreflect(ray_in.direction, rec.normal)
    attenuation = vec3(1, 1, 1)
    var refracted = vec3(0, 0, 0)
    var reflect_prob = 1f
    var outward_normal = rec.normal
    var ni_over_nt = 1f / self.ref_idx
    var dot = vdot(ray_in.direction, rec.normal)
    var len = length(ray_in.direction)
    var cosine = -dot / len
    if dot > 0f:
        outward_normal = -rec.normal
        ni_over_nt = self.ref_idx
        cosine = self.ref_idx * dot / len
    if refract(ray_in.direction, outward_normal, ni_over_nt, refracted):
        reflect_prob = schlick(cosine, self.ref_idx)
    if random_f() < reflect_prob:
        scattered = ray(rec.p, reflected)
    else:
        scattered = ray(rec.p, refracted)
    return true

proc scatter(self: Material, ray_in: Ray, rec: HitRec, attenuation: var Vec3,
        scattered: var Ray): bool =
    case self.kind
    of mkLambertian:
        return self.lambertian.scatter(ray_in, rec, attenuation, scattered)
    of mkMetal:
        self.metal.scatter(ray_in, rec, attenuation, scattered)
    of mkDielectric:
        self.dielectric.scatter(ray_in, rec, attenuation, scattered)

type Camera = object
    origin: Vec3
    lower_left_corner: Vec3
    horizontal: Vec3
    vertical: Vec3
    u, v, w: Vec3
    lens_radius: float32

proc get_ray(self: Camera, s, t: float32): Ray =
    var rd = self.lens_radius * random_in_unit_disk()
    var offset = rd.x * self.u + rd.y * self.v
    # var direction0 = t * self.vertical
    # var direction1 = s * self.horizontal
    # var direction = direction0 + direction1
    result = ray(self.origin + offset,
    self.lower_left_corner + s * self.horizontal + t * self.vertical -
    self.origin - offset)

func hsphere(center: Vec3, radius: float32, material: Material): Hittable =
    Hittable(kind: hkSphere, sphere: HSphere(center: center, radius: radius,
            material: material))

func hit_sphere(s: HSphere, r: Ray, t_min, t_max: float32,
    rec: var HitRec): bool =
    var oc = r.origin - s.center
    var a = vdot(r.direction, r.direction)
    var b = vdot(oc, r.direction)
    var c = vdot(oc, oc) - s.radius * s.radius
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

func make_camera(lookfrom: Vec3, lookat: Vec3, vup: Vec3, vfov,
        aspect, aperture, focus_dist: float32): Camera =
    const PI32 = float32(PI)
    var theta = vfov * PI32 / 180f
    var half_height = tan(theta / 2f)
    var half_width = aspect * half_height
    var w = unit_vector(lookfrom - lookat)
    var u = unit_vector(vcross(vup, w))
    var v = vcross(w, u)
    result = Camera(
        origin: lookfrom,
        lower_left_corner: lookfrom - half_width * focus_dist * u -
        half_height * focus_dist * v - focus_dist * w,
        horizontal: 2f * half_width * focus_dist * u,
        vertical: 2f * half_height * focus_dist * v,
        u: u, v: v, w: w,
        lens_radius: aperture / 2f
    )

proc vprint(v: Vec3) =
    stdout.write(&"{{{v.x:.6f}, {v.y:.6f}, {v.z:.6f};{cast[uint32](v.x):x}, {cast[uint32](v.y):x}, {cast[uint32](v.z):x}}}")

proc cam_print(cam: Camera) =
    (stdout.write("{\n\tlower_left_corner: "); vprint(cam.lower_left_corner);
             stdout.write(" "))
    (stdout.write("\n\thorizontal: "); vprint(cam.horizontal); stdout.write(" "))
    (stdout.write("\n\tvertical: "); vprint(cam.vertical); stdout.write(" "))
    (stdout.write("\n\torigin: "); vprint(cam.origin); stdout.write(" "))
    (stdout.write("\n}\n"))
    (stdout.write("\nu: "); vprint(cam.u); )
    (stdout.write("\nv: "); vprint(cam.v); )
    (stdout.write("\nw: "); vprint(cam.w); )
    (stdout.write(&"\nlens_radius={cam.lens_radius:.6f}\n"); )

pcg.srand(0)
var (nx, ny, ns) = (200, 100, 100)
echo(&"P3\n{nx} {ny}\n255")
var world: seq[Hittable] = @[
    hsphere(vec3(0, 0, -1), 0.5, lambertian(vec3(0.1, 0.2, 0.5))),
    hsphere(vec3(0, -100.5, -1), 100, lambertian(vec3(0.8, 0.8, 0))),
    hsphere(vec3(1, 0, -1), 0.5, metal(vec3(0.8, 0.6, 0.2), 0.3)),
    hsphere(vec3(-1, 0, -1), 0.5, dielectric(1.5)),
    hsphere(vec3(-1, 0, -1), -0.45, dielectric(1.5)),
]
var lookfrom = vec3(3, 3, 2)
var lookat = vec3(0, 0, -1)
var dist_to_focus = length(lookfrom - lookat)
var aperture = 2f
var cam = make_camera(lookfrom, lookat, vec3(0, 1, 0), 20, float32(nx) /
        float32(ny), aperture, dist_to_focus)
# cam_print(cam)
for j in countdown(ny - 1, 0):
    for i in countup(0, nx - 1):
        var col = vec3(0, 0, 0)
        # stdout.write(&"j={j} i={i}\n")
        for s in countup(0, ns-1):
            var u = (float32(i) + random_f()) / float32(nx)
            var v = (float32(j) + random_f()) / float32(ny)
            n_rand+=2
            # stdout.write(&"u={u:.6f} v={v:.6f}\n")
            # stdout.write(&"u={u:.6f;} v={v:.6f} nr={n_rand}\n")
            # var uv = vec3(u, v, 0)
            # stdout.write(&"uv={{{uv.x:.6f}, {uv.y:.6f}, {uv.z:.6f};{cast[uint32](uv.x):x}, {cast[uint32](uv.y):x}, {cast[uint32](uv.z):x}}} nr={n_rand}\n")
            var r = cam.get_ray(u, v)
            # stdout.write(&"r={{{{{r.origin.x:.6f}, {r.origin.y:.6f}, {r.origin.z:.6f};{cast[uint32](r.origin.x):x}, {cast[uint32](r.origin.y):x}, {cast[uint32](r.origin.z):x}}}, {{{r.direction.x:.6f}, {r.direction.y:.6f}, {r.direction.z:.6f};{cast[uint32](r.direction.x):x}, {cast[uint32](r.direction.y):x}, {cast[uint32](r.direction.z):x}}}}}\n")
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
