function main06_7() {
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
    function ray_color(r, world) {
        var ret = 0;
        for (const h of world) {
            if (h.t === 'sphere') {
                // log(`hitting sphere ${h}`)
                var temp_rec = {};
                if (hit_sphere(h.d, r, 0, infinity, temp_rec)) {
                    return vmul(0.5, vadd(temp_rec.normal, [1, 1, 1]));
                }
            }
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
        // world
        const world = [{ t: 'sphere', d: [[0, 0, -1], 0.5] }, { t: 'sphere', d: [[0, -100.5, -1], 100] }];
        // camera
        const viewport_height = 2.0;
        const viewport_width = aspect_ratio * viewport_height;
        const focal_length = 1.0;
        const origin = [0, 0, 0];
        const horizontal = [viewport_width, 0, 0];
        const vertical = [0, viewport_height, 0];
        const lower_left_corner = vsub(vsub(vsub(origin, vdiv(horizontal, 2.0)), vdiv(vertical, 2.0)), [0, 0, focal_length]);
        for (let j = 0; j < h; j++) {
            log(`Scanlines remaining: ${h - j - 1}`);
            for (let i = 0; i < w; i++) {
                const u = parseFloat(i) / (w - 1);
                const v = parseFloat(h - 1 - j) / (h - 1);
                const r = { orig: origin, dir: vsub(vadd(vadd(lower_left_corner, vmul(u, horizontal)), vmul(v, vertical)), origin) };
                const pixel_color = ray_color(r, world);
                image.data[(j * w + i) * 4 + 0] = 255.999 * pixel_color[0];
                image.data[(j * w + i) * 4 + 1] = 255.999 * pixel_color[1];
                image.data[(j * w + i) * 4 + 2] = 255.999 * pixel_color[2];
            }
        }
        ctx.putImageData(image, 0, 0);
        print(`Done. ${func}`);
    }
}
