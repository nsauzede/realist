#ifndef HITTABLEH
#define HITTABLEH

#include "ray.h"

extern int sph_hit;
extern int msph_hit;

class material;

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class aabb {
    public:
        aabb() {}
        aabb(const vec3& a, const vec3& b) { _min = a; _max = b;}

        vec3 min() const {return _min; }
        vec3 max() const {return _max; }

        bool hit(const ray& r, float tmin, float tmax) const {
            for (int a = 0; a < 3; a++) {
                float t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a],
                                 (_max[a] - r.origin()[a]) / r.direction()[a]);
                float t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a],
                                 (_max[a] - r.origin()[a]) / r.direction()[a]);
                tmin = ffmax(t0, tmin);
                tmax = ffmin(t1, tmax);
                if (tmax <= tmin)
                    return false;
            }
            return true;
        }

        vec3 _min;
        vec3 _max;
};

#if 0
inline bool aabb::hit(const ray& r, float tmin, float tmax) const {
    for (int a = 0; a < 3; a++) {
        float invD = 1.0f / r.direction()[a];
        float t0 = (min()[a] - r.origin()[a]) * invD;
        float t1 = (max()[a] - r.origin()[a]) * invD;
        if (invD < 0.0f)
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        if (tmax <= tmin)
            return false;
    }
    return true;
}
#endif

aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small( ffmin(box0.min().x(), box1.min().x()),
                ffmin(box0.min().y(), box1.min().y()),
                ffmin(box0.min().z(), box1.min().z()));
    vec3 big  ( ffmax(box0.max().x(), box1.max().x()),
                ffmax(box0.max().y(), box1.max().y()),
                ffmax(box0.max().z(), box1.max().z()));
    return aabb(small,big);
}

class hittable {
    public:
        virtual bool hit(
            const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
        virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};

class bvh_node : public hittable {
    public:
        bvh_node() {}
        bvh_node(hittable **l, int n, float time0, float time1);

        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;

        hittable *left;
        hittable *right;
        aabb box;
};

bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
    b = box;
    return true;
}

bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    if (box.hit(r, t_min, t_max)) {
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        }
        else if (hit_left) {
            rec = left_rec;
            return true;
        }
        else if (hit_right) {
            rec = right_rec;
            return true;
        }
        else
            return false;
    }
    else return false;
}

int box_x_compare (const void * a, const void * b) {
    aabb box_left, box_right;
    hittable *ah = *(hittable**)a;
    hittable *bh = *(hittable**)b;

    if (!ah->bounding_box(0,0, box_left) || !bh->bounding_box(0,0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";

    if (box_left.min().x() - box_right.min().x() < 0.0)
        return -1;
    else
        return 1;
}

int box_y_compare (const void * a, const void * b) {
    aabb box_left, box_right;
    hittable *ah = *(hittable**)a;
    hittable *bh = *(hittable**)b;

    if (!ah->bounding_box(0,0, box_left) || !bh->bounding_box(0,0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";

    if (box_left.min().y() - box_right.min().y() < 0.0)
        return -1;
    else
        return 1;
}

int box_z_compare (const void * a, const void * b) {
    aabb box_left, box_right;
    hittable *ah = *(hittable**)a;
    hittable *bh = *(hittable**)b;

    if (!ah->bounding_box(0,0, box_left) || !bh->bounding_box(0,0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";

    if (box_left.min().z() - box_right.min().z() < 0.0)
        return -1;
    else
        return 1;
}

bvh_node::bvh_node(hittable **l, int n, float time0, float time1) {
    int axis = int(3*random_double());

    if (axis == 0)
       qsort(l, n, sizeof(hittable *), box_x_compare);
    else if (axis == 1)
       qsort(l, n, sizeof(hittable *), box_y_compare);
    else
       qsort(l, n, sizeof(hittable *), box_z_compare);

    if (n == 1) {
        left = right = l[0];
    }
    else if (n == 2) {
        left = l[0];
        right = l[1];
    }
    else {
        left = new bvh_node(l, n/2, time0, time1);
        right = new bvh_node(l + n/2, n - n/2, time0, time1);
    }

    aabb box_left, box_right;

    if (!left->bounding_box(time0, time1, box_left) ||
        !right->bounding_box(time0, time1, box_right)) {

        std::cerr << "no bounding box in bvh_node constructor\n";
    }

    box = surrounding_box(box_left, box_right);
}

class material  {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation,
            ray& scattered) const = 0;
};

#endif
