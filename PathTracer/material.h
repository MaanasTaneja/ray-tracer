#pragma once
#include "util.h"

//We define what and how rays reflect and scatter in material structures.

class material {
public:
	virtual bool scatter(const ray& rayin, const intersection& intersect, vec3& attentuation, ray& rayout) = 0;
};

class diffuse : public material{
private:
	vec3 albedo;
public:
	diffuse(const vec3& col) : albedo{ col } {}

	virtual bool scatter(const ray& rayin, const intersection& intersect, vec3& attentuation, ray& rayout) {
		vec3 target_on_sphere = intersect.normal + random_in_hemisphere(intersect.normal);  //Yeah random in unit hemisphere but where?, I must provide reference vector, translate and orient at normal at hit point.
		vec3 scattered_direction = target_on_sphere - intersect.point;
		if (scattered_direction.near_zero())
			scattered_direction = intersect.normal;

		rayout = ray(intersect.point, (scattered_direction));
		attentuation = albedo;   //Color is what we would attentate the ray with (material absorbs some light and reflects rest)
		//thats how we see color
		return true;
	}
};

class metal : public material {
private: 
	vec3 albedo;
	double roughness;
public:
	metal(const vec3& col, double r) : albedo{ col }, roughness{ r } {}

	//Slightly create jitter in the reflected rays, random in unit sphere is toned down (less random angles) then oriented and translated by the orignal reflected vector, so we are around that only.
	virtual bool scatter(const ray& rayin, const intersection& intersect, vec3& attentuation, ray& rayout) {
		vec3 reflected_vector = reflect(unit_vector(rayin.getdirection()), intersect.normal);
		rayout = ray(intersect.point, reflected_vector + roughness * random_in_unit_sphere());
		attentuation = albedo;

		return true;
	}
};

class translucent :public material {
private:
	double ir;
	double frost;

	static double reflectance(double cosine, double ref_idx) {
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
public:
	translucent(double refractive_index, double frost_value) : ir{ refractive_index }, frost{ frost_value } {}

	virtual bool scatter(const ray& rayin, const intersection& intersect, vec3& attentuation, ray& rayout) {
		attentuation = vec3(1.0f, 1.0f, 1.0f); //No light attentuation as its transparent, no absorb.
		vec3 ray_in_unit = unit_vector(rayin.getdirection());

		double refraction_ratio = intersect.front_face ? (1.0 / ir) : ir;

		double cos_theta = fmin(dot(-ray_in_unit, intersect.normal), 1.0);
		double sin_theta = sqrt(1.0 - (cos_theta * cos_theta));

		vec3 scattered_dir;

		if ((refraction_ratio * sin_theta > 1.0) || (reflectance(cos_theta, refraction_ratio) > random_double())) {
			scattered_dir = reflect(ray_in_unit, intersect.normal);
		}
		else {
			scattered_dir = refract(ray_in_unit, intersect.normal, refraction_ratio) + frost * random_in_unit_sphere();
		}

		rayout = ray(intersect.point, scattered_dir);

		return true;
	}
};