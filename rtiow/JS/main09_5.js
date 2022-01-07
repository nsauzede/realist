function main09_5() {
    function hit_sphere(d, r, tmin, tmax, rec) {
        const center = d[0], radius = d[1];
        const oc = vsub(r.orig, center);
        const a = dot(r.dir, r.dir);
        const hb = dot(oc, r.dir);
        const c = dot(oc, oc) - radius * radius;
        const discriminant = hb * hb - a * c;
        if (discriminant < 0) return false;
        const sqrtd = Math.sqrt(discriminant);
        var root = (-hb - sqrtd) / a;
        if (root < tmin || root > tmax) {
            root = (-hb + sqrtd) / a;
            if (root < tmin || root > tmax) {
                return false;
            }
        }
        rec.t = root;
        rec.p = rat(r, rec.t);
        rec.normal = vdiv(vsub(rec.p, center), radius);
        return true;
    }
    function hit(world, r, tmin, tmax, rec) {
        var hit_anything = false;
        var closest_so_far = tmax;
        for (const h of world) {
            if (h.t === 'sphere') {
                if (hit_sphere(h.d, r, tmin, closest_so_far, rec)) {
                    hit_anything = true;
                    closest_so_far = rec.t;
                }
            }
        }
        return hit_anything;
    }
    function ray_color(r, world, depth) {
        var rec = {};
        if (depth <= 0) {
            // return [0, 0, 0];
            return [0, 0, 0];
        }
        if (hit(world, r, 0.001, infinity, rec)) {
            const target = vadd(rec.p, vadd(rec.normal, random_in_unit_sphere()));
            return vmul(0.5, ray_color({ orig: rec.p, dir: vsub(target, rec.p) }, world, depth - 1));
        }
        const unit_direction = unit_vector(r.dir);
        t = 0.5 * (unit_direction[1] + 1.0);
        return [1.0 - t + 0.5 * t, 1.0 - t + 0.7 * t, 1.0];
    }
    const func = arguments.callee.name || "anonymous";
    print(`${func}`);
    var canvas = document.getElementById('canvas');
    const w = canvas.width;
    const h = canvas.height;
    if (canvas.getContext) {
        var ctx = canvas.getContext('2d', { alpha: false });
        image = ctx.getImageData(0, 0, w, h);
        // image
        const aspect_ratio = 16.0 / 9.0;
        const image_width = w;
        const image_height = h;
        const samples_per_pixel = document.getElementById('spp').value;
        const max_depth = document.getElementById('max_depth').value;
        log(`using ${samples_per_pixel} SPP, ${max_depth} max depth`);
        const sppscale = 1.0 / samples_per_pixel;
        // world
        const material_ground = { t: 'lambertian', d: [0.8, 0.8, 0.0] };
        const material_center = { t: 'lambertian', d: [0.7, 0.3, 0.3] };
        const material_left = { t: 'metal', d: [0.8, 0.8, 0.8] };
        const material_right = { t: 'metal', d: [0.8, 0.6, 0.2] };
        var world = [];
        world.push({ t: 'sphere', d: [[0.0, -100.5, -1.0], 100.0], m: material_ground });
        world.push({ t: 'sphere', d: [[0.0, 0.0, -1.0], 0.5], m: material_center });
        world.push({ t: 'sphere', d: [[-1.0, 0.0, -1.0], 0.5], m: material_left });
        world.push({ t: 'sphere', d: [[1.0, 0.0, -1.0], 0.5], m: material_right });
        log(`world=${world}`);
        // camera
        const viewport_height = 2.0;
        const viewport_width = aspect_ratio * viewport_height;
        const focal_length = 1.0;
        const origin = [0, 0, 0];
        const horizontal = [viewport_width, 0, 0];
        const vertical = [0, viewport_height, 0];
        const lower_left_corner = vsub(vsub(vsub(origin, vdiv(horizontal, 2.0)), vdiv(vertical, 2.0)), [0, 0, focal_length]);
        for (let j = 0; j < h; j++) {
            // log(`Scanlines remaining: ${h - j - 1}`);
            for (let i = 0; i < w; i++) {
                var pixel_color = [0, 0, 0];
                for (let s = 0; s < samples_per_pixel; s++) {
                    const u = (parseFloat(i) + random_double()) / (w - 1);
                    const v = (parseFloat(h - 1 - j) + random_double()) / (h - 1);
                    const r = { orig: origin, dir: vsub(vadd(vadd(lower_left_corner, vmul(u, horizontal)), vmul(v, vertical)), origin) };
                    pixel_color = vadd(pixel_color, ray_color(r, world, max_depth));
                }
                image.data[(j * w + i) * 4 + 0] = 255.999 * Math.sqrt(pixel_color[0] * sppscale);
                image.data[(j * w + i) * 4 + 1] = 255.999 * Math.sqrt(pixel_color[1] * sppscale);
                image.data[(j * w + i) * 4 + 2] = 255.999 * Math.sqrt(pixel_color[2] * sppscale);
            }
        }
        ctx.putImageData(image, 0, 0);
        print(`Done. ${func}`);
    }
}
