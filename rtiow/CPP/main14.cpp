#include <iostream>
#define RANDOM_IMPL
#include "camera.h"
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "random.h"

void print_rec(hit_record& rec) {
	printf("{\n");
	printf("\tt: %f\n", rec.t);
	printf("\tp: "); rec.p.print(); printf("\n");
	printf("\tnormal: "); rec.normal.print(); printf("\n");
	printf("\tmat: *material type=%d*\n", rec.mat_ptr->type());
	printf("}\n");
//    vec3 p;
//    vec3 normal;
//    material *mat_ptr;
}

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation*color(scattered, world, depth+1);
        }
        else {
            return vec3(0,0,0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

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
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
        virtual int type() const {return 1;}
        vec3 albedo;
        float fuzz;
};
class lambertian : public material {
    public:
        lambertian(const vec3& a) : albedo(a) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec,
            vec3& attenuation, ray& scattered) const
        {
             vec3 target = rec.normal + random_in_unit_sphere();
             scattered = ray(rec.p, target);
             attenuation = albedo;
             return true;
        }
        virtual int type() const {return 0;}

        vec3 albedo;
};
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrtf(discriminant);
        return true;
    }
    else
        return false;
}
float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*powf((1 - cosine),5);
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

            if (random_f() < reflect_prob) {
               scattered = ray(rec.p, reflected);
            }
            else {
               scattered = ray(rec.p, refracted);
            }

            return true;
        }
        virtual int type() const {return 2;}

        float ref_idx;
};

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    const int N = 11; //11
    for (int a = -N; a < N; a++) {
        for (int b = -N; b < N; b++) {
            float choose_mat = random_f();
//    if (i < 3)fprintf(stderr, "choose_mat=%f\n", choose_mat);
            float r1 = random_f();
            float r2 = random_f();
            vec3 center(a+0.9*r2,0.2,b+0.9*r1);
//            fprintf(stderr, "a=%d b=%d center={%f, %f. %f}\n", a, b, center.x(), center.y(), center.z());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
            float r1 = random_f();
            float r2 = random_f();
            float r3 = random_f();
            float r4 = random_f();
            float r5 = random_f();
            float r6 = random_f();
                    list[i++] = new sphere(center, 0.2,
                        new lambertian(vec3(r6*r5,r4*r3,r2*r1)
                        )
                    );
                }
                else if (choose_mat < 0.95) { // metal
            float r1 = random_f();
            float r2 = random_f();
            float r3 = random_f();
            float r4 = random_f();
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + r4),
                                           0.5*(1 + r3),
                                           0.5*(1 + r2)),
                                      0.5*r1));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
}
//#define DEBUG
int main(int argc, char *argv[]) {
	srand(0);
	char *fnameout = 0;
	FILE *fout = stdout;
	int nx = 200;
	int ny = 100;
	int ns = 4;
	int arg = 1;
	if (arg < argc) {
		sscanf(argv[arg++], "%d", &nx);
		if (arg < argc) {
			sscanf(argv[arg++], "%d", &ny);
			if (arg < argc) {
				sscanf(argv[arg++], "%d", &ns);
				if (arg < argc) {
					fnameout = argv[arg++];
				}
			}
		}
	}
	unsigned char *bytes = 0;
	size_t nbytes = 0;
	if (fnameout) {
	fout = fopen(fnameout, "wb");
		fprintf(fout, "P6\n");
		nbytes = 3 * ny * nx;
		bytes = (unsigned char *)malloc(nbytes);
	} else {
		fprintf(fout, "P3\n");
	}
	fprintf(fout, "%d %d\n", nx, ny);
	fprintf(fout, "255\n");
	hitable *world = random_scene();
	vec3 lookfrom(9, 2, 2.6);
	vec3 lookat(3, 0.8, 1);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 0.0;
	camera cam(
		lookfrom,
		lookat,
		vec3(0, 1, 0),
		30, (float)nx/(float)ny,
		aperture,
		dist_to_focus);
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col(0, 0, 0);
			for (int s=0; s < ns; s++) {
				float u = ((float)i + random_f()) / (float)nx;
				float v = ((float)j + random_f()) / (float)ny;
//				printf("u=%f v=%f ", u, v);
				ray r = cam.get_ray(u, v);
//				printf("r=");r.print();printf(" ");
//				col += color(r, world, 0);
				vec3 c0 = color(r, world, 0);
//				c0.print();
				col += c0;
			}
//			col.print();
			col /= (float)ns;
			col = vec3( sqrtf(col[0]), sqrtf(col[1]), sqrtf(col[2]) );
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);
			if (fnameout) {
				bytes[((ny - 1 - j) * nx + i) * 3 + 0] = ir;
				bytes[((ny - 1 - j) * nx + i) * 3 + 1] = ig;
				bytes[((ny - 1 - j) * nx + i) * 3 + 2] = ib;
			} else {
				fprintf(fout, "%d %d %d  ", ir, ig, ib);
			}
		}
		if (!fnameout) {
			fprintf(fout, "\n");
		}
	}
	if (fnameout) {
		fwrite(bytes, nbytes, 1, fout);
		fclose(fout);
		free(bytes);
	}
}
