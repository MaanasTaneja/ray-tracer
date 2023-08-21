#pragma once
#include <iostream>
#include "util.h"
#include "scene.h"
#include "camera.h"
#include "material.h"

struct renderer_desc_t {
	int16_t WIDTH;
	double ASPECT_RATIO;

	int RAY_BOUNCE_LIMIT;
	double RAY_SAMPLES_PER_PIXEL;

	double CAMERA_VERTICAL_FOV;
	vec3 CAMERA_LOOK_FROM;
	vec3 CAMERA_LOOK_AT;
};

class renderer {
private:
	renderer_desc_t settings;
	Image* image;  //Question, does not seem to work while copying directly, oh wait i know why, because its a dumb shallow copy, wont be proper data simply pointers?
	//But i thought i was initlaizing the image object in the constrcutor? no copying involved no? thats interesting im not sure why that idnt work and this had to be a pointer to work.
	camera cam;
	scene world;

	/*
	Ray color - How does it work?

	Ray is traced, and finds closest object along ray's path (origin , direction), how does light work in real life?
	When light rays strike an object the object absorbs some of the light and reflects the rest. Essentially what we do here also!
	Ray hit is registered and then we multiply the bounced ray with the objects color! (which is 0 to 1) thus dimishing the rays energy
	and absorbing what we need to and letting the rest go!
	then so on and so forth.

	*/

	vec3 ray_color(const ray& r, const scene& world, int depth) {
		intersection closest;
		//Depth tracks number of bounces (recursion)
		if (depth <= 0)
			return vec3(0, 0, 0);
		// If we've exceeded the ray bounce limit, no more light is gathered.

		if (world.hitTest(r, 0.001f, infinity, closest) == true) {
			ray scattered;
			vec3 attenuation_to_ray; //Essentially color of object we just hit
			closest.hit_obj_material->scatter(r, closest, attenuation_to_ray, scattered);
			return  attenuation_to_ray * ray_color(scattered, world, depth - 1);
		}

		vec3 unit_direction = unit_vector(r.getdirection());
		double t = 0.5 * (unit_direction.gety() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}


	//Iteration is much slower than recursive solution. Why???? I am so confused!??
	vec3 ray_color_iter(ray& r, const scene& world, int depth) {
		bool hitanything = false;

		intersection currentBounce;
		vec3 color{ 1.0f, 1.0f, 1.0f }; //Starting ray color is white (max energy)

		for (int i = 0; i < depth; i++) {
			if (color == vec3(0.0, 0.0, 0.0)) {
				return vec3(0.0, 0.0, 0.0);
			}
			if (world.hitTest(r, 0.001f, infinity, currentBounce) == true) {
				vec3 attenuation_to_ray; //Essentially color of object we just hit
				currentBounce.hit_obj_material->scatter(r, currentBounce, attenuation_to_ray, r);
				color = attenuation_to_ray * color; //Taking light energy and coloring the light (based on what we absorbed)
				hitanything = true;
			}
		}
		if (!hitanything) {
			vec3 unit_direction = unit_vector(r.getdirection());
			double t = 0.5 * (unit_direction.gety() + 1.0);
			return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
		}

		return color;

	}


public:

	renderer(const renderer_desc_t& init, const scene& w) {
		settings = init;
		image = new Image{ settings.WIDTH, static_cast<double>(settings.ASPECT_RATIO) };
		cam = camera{ settings.CAMERA_LOOK_FROM, settings.CAMERA_LOOK_AT, vec3(0.0f, 1.0f, 0.0f), settings.CAMERA_VERTICAL_FOV, settings.ASPECT_RATIO, settings.RAY_SAMPLES_PER_PIXEL };
		world = w;
	}

	void render() {
		for (int j = image->getheight() - 1; j >= 0; --j) {
			std::cout << "\rScanlines remaining: " << j << ' ' << std::endl;
			for (int i = 0; i < image->getwidth(); ++i) {
				//to sample each pixels number of times (for AA etc), we will add all color samples 
				//(casting rays at same pixel with jitter multiple times), then divide final color by spp.
				vec3 pixel_color{ 0.0f };
				//Each pixel is ray casted multiple times.
				for (int s = 0; s < cam.getspp(); s++) {
					double u = ((i)+random_double()) / (image->getwidth() - 1); //x percent 
					double v = ((j)+random_double()) / (image->getheight() - 1); // y percent

					ray r = cam.get_ray_to_screen(u, v);
					pixel_color = pixel_color + ray_color(r, world, settings.RAY_BOUNCE_LIMIT);
				}
				image->write_color_sampled(pixel_color, cam.getspp());
			}
		}
	}

	Image* getImage() { return image; }
};