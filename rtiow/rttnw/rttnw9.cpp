#include <iostream>
#include <cfloat>

#include "camera.h"
#include "sphere.h"
#include "hittable_list.h"
#include "random.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

vec3 color(const ray& r, hittable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
             return emitted + attenuation*color(scattered, world, depth+1);
        else
            return emitted;
    }
    else {
#if 1
        // no background -- dark night
        return vec3(0,0,0);
#else
        // ambient daylight (sky)
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
#endif
    }
}

class texture {
    public:
        virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
    public:
        constant_texture() {}
        constant_texture(vec3 c) : color(c) {}
        virtual vec3 value(float u, float v, const vec3& p) const {
            return color;
        }
        vec3 color;
};

class checker_texture : public texture {
    public:
        checker_texture() {}
        checker_texture(texture *t0, texture *t1): even(t0), odd(t1) {}
        virtual vec3 value(float u, float v, const vec3& p) const {
            float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
            if (sines < 0)
                return odd->value(u, v, p);
            else
                return even->value(u, v, p);
        }
        texture *even;
        texture *odd;
};

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

class metal : public material {
    public:
        metal(const vec3& a, float f) : albedo(a) {
            if (f < 1) fuzz = f; else fuzz = 1;
        }

        virtual bool scatter(const ray& r_in, const hit_record& rec,
            vec3& attenuation, ray& scattered) const
        {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
        vec3 albedo;
        float fuzz;
};

class lambertian : public material {
    public:
        // albedo is the measure of the diffuse reflection of solar radiation
        // out of the total solar radiation received by an astronomical body
        // surface albedo : ratio of radiosity to the irradiance received by a surface
        lambertian(texture *a) : albedo(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec,
            vec3& attenuation, ray& scattered) const {

             vec3 target = rec.p + rec.normal + random_in_unit_sphere();
             scattered = ray(rec.p, target - rec.p, r_in.time());
             attenuation = albedo->value(rec.u, rec.v, rec.p);
             return true;
        }

        texture *albedo;
};

class diffuse_light : public material {
    public:
        diffuse_light(texture *a) : emit(a) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec,
            vec3& attenuation, ray& scattered) const { return false; }
        virtual vec3 emitted(float u, float v, const vec3& p) const {
            return emit->value(u, v, p);
        }
        texture *emit;
};

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    else
        return false;
}

float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}

class dielectric : public material {
    public:
        dielectric(float ri) : ref_idx(ri) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec,
                             vec3& attenuation, ray& scattered) const {
            vec3 outward_normal;
            vec3 reflected = reflect(r_in.direction(), rec.normal);
            float ni_over_nt;
            attenuation = vec3(1.0, 1.0, 1.0);
            vec3 refracted;

            float reflect_prob;
            float cosine;

            if (dot(r_in.direction(), rec.normal) > 0) {
                 outward_normal = -rec.normal;
                 ni_over_nt = ref_idx;
                 cosine = ref_idx * dot(r_in.direction(), rec.normal)
                        / r_in.direction().length();
            }
            else {
                 outward_normal = rec.normal;
                 ni_over_nt = 1.0 / ref_idx;
                 cosine = -dot(r_in.direction(), rec.normal)
                        / r_in.direction().length();
            }

            if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
               reflect_prob = schlick(cosine, ref_idx);
            }
            else {
               reflect_prob = 1.0;
            }

            if (random_double() < reflect_prob) {
               scattered = ray(rec.p, reflected, r_in.time());
            }
            else {
               scattered = ray(rec.p, refracted, r_in.time());
            }

            return true;
        }

        float ref_idx;
};

class image_texture : public texture {
    public:
        image_texture() {}
        image_texture(unsigned char *pixels, int A, int B)
            : data(pixels), nx(A), ny(B) {}
        virtual vec3 value(float u, float v, const vec3& p) const;
        unsigned char *data;
        int nx, ny;
};

vec3 image_texture::value(float u, float v, const vec3& p) const {
     int i = (  u) * nx;
     int j = (1-v) * ny - 0.001;
     if (i < 0) i = 0;
     if (j < 0) j = 0;
     if (i > nx-1) i = nx-1;
     if (j > ny-1) j = ny-1;
     float r = int(data[3*i + 3*nx*j]  ) / 255.0;
     float g = int(data[3*i + 3*nx*j+1]) / 255.0;
     float b = int(data[3*i + 3*nx*j+2]) / 255.0;
     return vec3(r, g, b);
}

hittable *random_scene() {
    int n = 50000;
    hittable **list = new hittable*[n+1];
    hittable **slist = new hittable*[n+1];
    int i = 0;
#if 1
    texture *checker = new checker_texture(
        new constant_texture(vec3(0.2, 0.3, 0.1)),
        new constant_texture(vec3(0.9, 0.9, 0.9)));
#else
    texture *checker = new constant_texture(vec3(0.5, 0.5, 0.5));
#endif
#if 1
    list[i++] =  new sphere(vec3(0,-1000,0), 1000, new diffuse_light(checker));
#elif 1
    list[i++] =  new sphere(vec3(0,-1000,0), 1000, new diffuse_light(new constant_texture(vec3(4,4,4))));
#else
int nx, ny, nn;
unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
    list[i++] =  new sphere(vec3(0,-1000,0), 1000, new diffuse_light(new image_texture(tex_data, nx, ny)));
#endif
    int si = 0;
/*
-10 -9 -8 -7 -6 -5 -4 -3 -2 -1 0
1 2 3 4 5 6 7 8 9
20*20=400
*/
    for (int a = -10; a < 10; a++) {
//        fprintf(stderr, "%d ", a);
        for (int b = -10; b < 10; b++) {
            float choose_mat = random_double();
            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    slist[si++] = new moving_sphere(
                        center,
                        center+vec3(0, 0.5*random_double(), 0),
                        0.0, 1.0, 0.2,
#if 1
                        new lambertian(
                            new constant_texture(vec3(random_double()*random_double(),
                                 random_double()*random_double(),
                                 random_double()*random_double())
                        ))
#else
                        new dielectric(1.5)
#endif
                    );
                }
                else if (choose_mat < 0.95) { // metal
                    slist[si++] = new sphere(
                        center, 0.2,
                        new metal(
                            vec3(0.5*(1 + random_double()),
                                 0.5*(1 + random_double()),
                                 0.5*(1 + random_double())),
                            0.5*random_double()
                        )
                    );
                }
                else {  // glass
                    slist[si++] = new sphere(center, 0.2, 
#if 1
                    new dielectric(1.5)
#else
                        new lambertian(
                            new constant_texture(vec3(random_double()*random_double(),
                                 random_double()*random_double(),
                                 random_double()*random_double())
                        ))
#endif
                    );
                }
            }
        }
    }

    list[i++] = new bvh_node(slist, si, 0, 1);
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    fprintf(stderr, "Created scene with %d hittables\n", i);
    return new hittable_list(list,i);
}

hittable *two_spheres() {
    texture *checker = new checker_texture(
        new constant_texture(vec3(0.2, 0.3, 0.1)),
        new constant_texture(vec3(0.9, 0.9, 0.9))
    );
    int n = 50;
    hittable **list = new hittable*[n+1];
//    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(checker));
//    list[0] =  new sphere(vec3(0,-1000,0), 1000, new diffuse_light(new constant_texture(vec3(4,4,4))));
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new diffuse_light(checker));
    list[1] = new sphere(vec3(0, 1, 0), 1.0, new lambertian(checker));
//    list[1] = new sphere(vec3(0, 1, 0), 1.0, new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new hittable_list(list,2);
}

inline float trilinear_interp(float c[2][2][2], float u, float v, float w) {
    float accum = 0;
    for (int i=0; i < 2; i++)
        for (int j=0; j < 2; j++)
            for (int k=0; k < 2; k++)
                accum += (i*u + (1-i)*(1-u))*
                         (j*v + (1-j)*(1-v))*
                         (k*w + (1-k)*(1-w))*c[i][j][k];

    return accum;
}

inline float perlin_interp(vec3 c[2][2][2], float u, float v, float w) {
    float uu = u*u*(3-2*u);
    float vv = v*v*(3-2*v);
    float ww = w*w*(3-2*w);
    float accum = 0;
    for (int i=0; i < 2; i++)
        for (int j=0; j < 2; j++)
            for (int k=0; k < 2; k++) {
                vec3 weight_v(u-i, v-j, w-k);
                accum += (i*uu + (1-i)*(1-uu))*
                    (j*vv + (1-j)*(1-vv))*
                    (k*ww + (1-k)*(1-ww))*dot(c[i][j][k], weight_v);
            }
    return accum;
}

class perlin {
    public:
        float noise(const vec3& p) const {
            float u = p.x() - floor(p.x());
            float v = p.y() - floor(p.y());
            float w = p.z() - floor(p.z());
            int i = floor(p.x());
            int j = floor(p.y());
            int k = floor(p.z());
            vec3 c[2][2][2];
            for (int di=0; di < 2; di++)
                for (int dj=0; dj < 2; dj++)
                    for (int dk=0; dk < 2; dk++)
                        c[di][dj][dk] = ranvec[
                            perm_x[(i+di) & 255] ^
                            perm_y[(j+dj) & 255] ^
                            perm_z[(k+dk) & 255]
                        ];
            return perlin_interp(c, u, v, w);
        }
        static vec3 *ranvec;
        static int *perm_x;
        static int *perm_y;
        static int *perm_z;

float turb(const vec3& p, int depth=7) const {
    float accum = 0;
    vec3 temp_p = p;
    float weight = 1.0;
    for (int i = 0; i < depth; i++) {
        accum += weight*noise(temp_p);
        weight *= 0.5;
        temp_p *= 2;
    }
    return fabs(accum);
}

};

static vec3* perlin_generate() {
    vec3 *p = new vec3[256];
    for (int i = 0; i < 256; ++i) {
        double x_random = 2*random_double() - 1;
        double y_random = 2*random_double() - 1;
        double z_random = 2*random_double() - 1;
        p[i] = unit_vector(vec3(x_random, y_random, z_random));
    }
    return p;
}

void permute(int *p, int n) {
    for (int i = n-1; i > 0; i--) {
        int target = int(random_double()*(i+1));
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
    return;
}

static int* perlin_generate_perm() {
    int * p = new int[256];
    for (int i = 0; i < 256; i++)
        p[i] = i;
    permute(p, 256);
    return p;
}

vec3 *perlin::ranvec = perlin_generate();
int *perlin::perm_x = perlin_generate_perm();
int *perlin::perm_y = perlin_generate_perm();
int *perlin::perm_z = perlin_generate_perm();

class noise_texture : public texture {
    public:
        noise_texture() {}
        noise_texture(float sc) : scale(sc) {}
        virtual vec3 value(float u, float v, const vec3& p) const {
#if 0
            // straight noise 
            return vec3(1,1,1) * noise.noise(scale * p);
#elif 0
            // direct turbulence => camouflage netting appearance
            return vec3(1,1,1) * noise.turb(scale * p);
#else
            // indirect sine undulate turbulence => marble
            return vec3(1,1,1) * 0.5 * (1 + sin(scale*p.z() + 10*noise.turb(p)));
#endif
        }
        perlin noise;
        float scale;
};

hittable *two_perlin_spheres() {
#if 1
    texture *pertext = new noise_texture(4);
#else
    texture *pertext = new checker_texture(
        new constant_texture(vec3(0.2, 0.3, 0.1)),
        new constant_texture(vec3(0.9, 0.9, 0.9))
    );
#endif
    hittable **list = new hittable*[2];
    list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    return new hittable_list(list, 2);
}

hittable *two_tex_spheres() {
int nx, ny, nn;
unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
material *mat = new lambertian(new image_texture(tex_data, nx, ny));

    hittable **list = new hittable*[2];
    list[1] = new sphere(vec3(0, 2, 0), 2, mat);
    list[0] = new sphere(vec3(0,-1000, 0), 1000, new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new hittable_list(list, 2);
}

hittable *simple_light() {
int nx, ny, nn;
unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
material *mat = new lambertian(new image_texture(tex_data, nx, ny));
    texture *pertext = new noise_texture(4);
    hittable **list = new hittable*[4];
    int i = 0;
    list[i++] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
//    list[i++] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    list[i++] = new sphere(vec3(0, 2, 0), 2, mat);
#if 1
    list[i++] = new sphere(vec3(0, 7, 0), 2,
        new diffuse_light(new constant_texture(vec3(4,4,4))));
#endif
    list[i++] = new xy_rect(3, 5, 1, 3, -2,
        new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new hittable_list(list,i);
}

hittable *cornell_box() {
    hittable **list = new hittable*[100];
    int i = 0;
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *blue = new lambertian(new constant_texture(vec3(0.05, 0.05, 0.65)));
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
//    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, blue));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
#if 1
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
#endif
#if 0
//    list[i++] = new box(vec3(130, 0, 65), vec3(295, 165, 230), white);
    list[i++] = new box(vec3(130, 0, 65), vec3(295, 165, 230), blue);
    list[i++] = new box(vec3(265, 0, 295), vec3(430, 330, 460), white);
#else
list[i++] = new translate(
                new rotate_y(new box(vec3(0,0,0), vec3(165,165,165), white), -18),
                vec3(130,0,65)
            );
list[i++] = new translate(
                new rotate_y(new box(vec3(0,0,0), vec3(165,330,165), white), 15),
                vec3(265,0,295)
            );
#endif
#if 0
const int N = 10;
    list[i++] = new sphere(vec3(0, 0, 0), N, white);
    list[i++] = new sphere(vec3(N, 0, 0), N, red);
    list[i++] = new sphere(vec3(0, N, 0), N, green);
    list[i++] = new sphere(vec3(0, 0, N), N, blue);
#endif

    return new hittable_list(list,i);
}

class isotropic : public material {
    public:
        isotropic(texture *a) : albedo(a) {}
        virtual bool scatter(
            const ray& r_in,
            const hit_record& rec,
            vec3& attenuation,
            ray& scattered) const {

            scattered = ray(rec.p, random_in_unit_sphere());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }
        texture *albedo;
};

class constant_medium : public hittable {
    public:
        constant_medium(hittable *b, float d, texture *a) : boundary(b), density(d) {
            phase_function = new isotropic(a);
        }
        virtual bool hit(
            const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const {
            return boundary->bounding_box(t0, t1, box);
        }
        hittable *boundary;
        float density;
        material *phase_function;
};

bool constant_medium::hit(const ray& r, float t_min, float t_max, hit_record& rec)
const {

    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    bool debugging = enableDebug && random_double() < 0.00001;

    hit_record rec1, rec2;

    if (boundary->hit(r, -FLT_MAX, FLT_MAX, rec1)) {
        if (boundary->hit(r, rec1.t+0.0001, FLT_MAX, rec2)) {

            if (debugging) std::cerr << "\nt0 t1 " << rec1.t << " " << rec2.t << '\n';

            if (rec1.t < t_min)
                rec1.t = t_min;

            if (rec2.t > t_max)
                rec2.t = t_max;

            if (rec1.t >= rec2.t)
                return false;

            if (rec1.t < 0)
                rec1.t = 0;

            float distance_inside_boundary = (rec2.t - rec1.t)*r.direction().length();
            float hit_distance = -(1/density) * log(random_double());

            if (hit_distance < distance_inside_boundary) {

                rec.t = rec1.t + hit_distance / r.direction().length();
                rec.p = r.point_at_parameter(rec.t);

                if (debugging) {
                    std::cerr << "hit_distance = " <<  hit_distance << '\n'
                              << "rec.t = " <<  rec.t << '\n'
                              << "rec.p = " <<  rec.p << '\n';
                }

                rec.normal = vec3(1,0,0);  // arbitrary
                rec.mat_ptr = phase_function;
                return true;
            }
        }
    }
    return false;
}

hittable *cornell_smoke() {
    hittable **list = new hittable*[8];
    int i = 0;
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

    hittable *b1 = new translate(
        new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
        vec3(130,0,65));
    hittable *b2 = new translate(
        new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15),
        vec3(265,0,295));

    list[i++] = new constant_medium(
        b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
    list[i++] = new constant_medium(
        b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));

    return new hittable_list(list,i);
}

int sph_hit = 0;
int msph_hit = 0;

int main(int argc, char *argv[]) {
    srand(0);
    int nx = 200;//200
    int ny = 100;//100
    int ns = 100;//100
    int arg = 1;
    if (arg < argc) {
        sscanf(argv[arg++], "%d", &nx);
        if (arg < argc) {
            sscanf(argv[arg++], "%d", &ny);
            if (arg < argc) {
                sscanf(argv[arg++], "%d", &ns);
            }
        }
    }
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
#if 1
//hittable *world = cornell_box();
hittable *world = cornell_smoke();

vec3 lookfrom(278, 278, -800);
vec3 lookat(278,278,0);
float dist_to_focus = 10.0;
float aperture = 0.0;
float vfov = 40.0;

camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny),
    aperture, dist_to_focus, 0.0, 1.0);
#elif 1
hittable *world = simple_light();

vec3 lookfrom(6,2,3);
vec3 lookat(0,2,0);
float dist_to_focus = 10.0;
float aperture = 0.0;
camera cam(lookfrom, lookat, vec3(0,1,0), 50, float(nx)/float(ny),
           aperture, dist_to_focus, 0.0, 1.0);
#elif 1
    hittable *world = random_scene();

vec3 lookfrom(13,2,3);
vec3 lookat(0,0,0);
float dist_to_focus = 10.0;
float aperture = 0.0;

camera cam(
    lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture,
    dist_to_focus, 0.0, 1.0
);
#elif 1
    hittable *world = two_spheres();

vec3 lookfrom(10,1.5,3);
vec3 lookat(0,0.4,0);
float dist_to_focus = 10.0;
float aperture = 0.0;

camera cam(
    lookfrom, lookat, vec3(0,1,0), 13, float(nx)/float(ny), aperture,
    dist_to_focus, 0.0, 1.0
);
#elif 0
    hittable *world = two_perlin_spheres();

vec3 lookfrom(13,2,3);
vec3 lookat(0,0,0);
float dist_to_focus = 10.0;
float aperture = 0.0;
camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny),
           aperture, dist_to_focus, 0.0, 1.0);
#else
    hittable *world = two_tex_spheres();

vec3 lookfrom(13,2,3);
vec3 lookat(0,0,0);
float dist_to_focus = 10.0;
float aperture = 0.0;
camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny),
           aperture, dist_to_focus, 0.0, 1.0);
#endif

    int gsph_hit = 0, gmsph_hit = 0;
    time_t t0 = time(0);
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i + random_double()) / float(nx);
                float v = float(j + random_double()) / float(ny);
                ray r = cam.get_ray(u, v);
                sph_hit = 0;
                msph_hit = 0;
                col += color(r, world,0);
                gsph_hit += sph_hit; gmsph_hit += msph_hit;
//                fprintf(stderr, "sph_hit=%d msph_hit=%d total=%d\n", sph_hit, msph_hit, sph_hit + msph_hit);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            if (ir < 0) ir = 255;
            if (ig < 0) ig = 255;
            if (ib < 0) ib = 255;
            std::cout << ir << " " << ig << " " << ib << " ";
        }
        std::cout << "\n";
    }
    time_t t1 = time(0);
    int ti = t1 - t0;
    double sp = (double)(gsph_hit + gmsph_hit) / ti;
    fprintf(stderr, "sph_hit=%d msph_hit=%d total=%d time=%d speed=%.2f hit/sec\n", gsph_hit, gmsph_hit, gsph_hit + gmsph_hit, ti, sp);
}
