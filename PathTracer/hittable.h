#pragma once
#include "util.h"

class material;

struct intersection {
	vec3 point;
	vec3 normal;
	double t;
	bool front_face;

	std::shared_ptr<material> hit_obj_material;

	//In this tracer, we assume outward facing normals or normals against ray dir as standard.
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.getdirection(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
		//If ray and normal against each other then its front
		//else it isnt and normal must be -ve
	}
};

class hittable {
public:
	//Most ray tracers have found it convenient to add a valid interval for hits tmin
	//to tmax, so the hit only “counts” if tmin < t < tmax

	virtual bool hit(const ray& r, double t_min, double t_max, intersection& rec) = 0;
};

class sphere : public hittable {
private:
	vec3 centre;
	double radius;
	std::shared_ptr<material> mat_ptr;
public:
	sphere(vec3 c, double rad, std::shared_ptr<material> mat) : centre{ c }, radius{ rad }, mat_ptr{ mat } {}

	virtual bool hit(const ray& ray_, double t_min, double t_max, intersection& intersect) {
		//We must solve the quadratic eqn for a spehre in space with this line (ray) and see if it intersects, touches or misses.
		//Eq : t2b.b + 2tb.(A−C) + (A−C).(A−C) − r2=0, solve in terms of t
		//Discriminat  - sqrt(b2-4ac), if discrm >= 0 ray hits, else, miss.
		//(A-C) is simply origin of ray - sphere centre

		vec3 ac = ray_.getorigin() - centre;
		double a = (dot(ray_.getdirection(), ray_.getdirection()));
		double b = dot(ray_.getdirection(), ac) * 2.0;
		double c = dot(ac, ac) - radius * radius;

		double discriminant = (b * b - 4 * a * c);
		if (discriminant < 0) { return false; }

		double sqrt_d = std::sqrt(discriminant);
		//Now two roots exist of a positive intersecting relation as this is quadratic
		//ray in intersection, and ray out intersection
		//We choose the closest and FIRST intersection as our valid intersection
		//As we are interested in entry intersection only.

		double root = (-b - sqrt_d) / (2 * a);
		if (root < t_min || root > t_max) {
			//If first selected root is out of bounds, the other root may be the one we want.
			root = (-b + sqrt_d) / (2 * a);
				if (root < t_min || root > t_max) {
					//If second root is invalid also, simply false.
					return false;
			}
		}

		intersect.t = root;
		intersect.point = ray_.pointat(intersect.t);
		vec3 outward_normal = (intersect.point - centre) / radius; //Normal Unit vector.
		intersect.set_face_normal(ray_, outward_normal);
		intersect.hit_obj_material = mat_ptr;
		//If this sphere hits, then pass its material into intersection struct
		return true;
	}
};