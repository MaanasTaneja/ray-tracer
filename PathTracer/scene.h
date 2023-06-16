#pragma once
#include "hittable.h"
#include <vector>
#include <memory>

class scene {
private:
	//Hittable has to be a pointer type, or else we can't do any polymorphism.
	//and with shared pointers, I dont have to rule of 3 or whatever, because memory is manged by sharedptr!
	//If i do this using raw pointers, then I would need all copy constrcuors/assigments move semantics, and ugly
	//pointer functions

	std::vector<std::shared_ptr<hittable>> objects;
public:
	scene() {}
	scene(std::vector<std::shared_ptr<hittable>> list) : objects{ list } {}

	void clear() {
		objects.clear();
	}

	void add(std::shared_ptr<hittable> obj) {
		objects.push_back(obj);
	}

	//Goes through every object and finds the closest hit on a particular input ray.
	bool hitTest(const ray& ray_, double t_min, double t_max, intersection& intersect) const {
		//We must find the closest intersection.
		intersection temp_intersect;
		bool hit_anything = false;
		auto closest_so_far = t_max; //Max point on ray is closest (infinty)

		for (std::shared_ptr<hittable> obj : objects) {
			//Loop through all objects and keep setting t max, as it gets smaller and smaller\
			//until we find the closest object which intersects the ray.
			//We loop thrugh all objects and any of them can be anywhere on the ray
			//t_max tells us which intersection (and uptill where) was the furthest along
			//ray and we should not check the ray after t_max

			if (obj->hit(ray_, t_min, closest_so_far, temp_intersect)) {
				hit_anything = true;
				closest_so_far = temp_intersect.t;
				intersect = temp_intersect;
			}
		}
		return hit_anything;
	}

};