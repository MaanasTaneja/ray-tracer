// PathTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <fstream>
#include "scene.h"
#include "camera.h"
#include "graphics.h"
#include "material.h"

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

    if (world.hitTest(r, 0.000001f, infinity, closest) == true) {
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
        if (world.hitTest(r, 0.000001f, infinity, currentBounce) == true) {
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

int main()
{
    const int max_depth = 10;
    Image image{ 720, static_cast<double> (16.0/9.0)};

    camera cam{ image.getaspectratio(), 100 };

    scene world{};
    std::shared_ptr<diffuse> material_ground = std::make_shared<diffuse>(vec3(0.8, 0.8, 0.0));
    std::shared_ptr<diffuse> material_center = std::make_shared<diffuse>(vec3(0.7, 0.3, 0.3));
    std::shared_ptr<translucent> material_left = std::make_shared<translucent>(1.5, 0.0);
    std::shared_ptr<translucent> material_right = std::make_shared<translucent>(1.5, 0.5);

    world.add(std::make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(std::make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(std::make_shared<sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));

    for (int j = image.getheight() - 1; j >= 0; --j) {
        for (int i = 0; i < image.getwidth(); ++i) {
            //to sample each pixels number of times (for AA etc), we will add all color samples 
            //(casting rays at same pixel with jitter multiple times), then divide final color by spp.
            vec3 pixel_color{ 0.0f };
            //Each pixel is ray casted multiple times.
            for (int s = 0; s < cam.getspp(); s++) {
                double u = ((i) + random_double()) / (image.getwidth() - 1); //x percent 
                double v = ((j) + random_double()) / (image.getheight() - 1); // y percent

                ray r = cam.get_ray_to_screen(u, v);
                pixel_color = pixel_color + ray_color(r, world, max_depth);
            }
            image.write_color_sampled(pixel_color, cam.getspp());
        }
    }

    Graphics gfx{ image.width, image.height };
    gfx.init();
    gfx.error_check();


    while (!gfx.livestatus())
    {
        gfx.startFrame();

        gfx.bindScreenQuad();
        gfx.bindTexture((image.pixel_data.address()), image.width, image.height);
        gfx.draw();
        
        gfx.endFrame();
    }

	return 0;

}
