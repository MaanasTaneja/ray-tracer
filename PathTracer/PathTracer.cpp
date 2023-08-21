// PathTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <fstream>
#include "renderer.h"
#include "graphics.h"


scene random_scene() {
    scene world;

    auto ground_material = std::make_shared<diffuse>(vec3(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(vec3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    sphere_material = std::make_shared<diffuse>(albedo);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = std::make_shared<translucent>(1.5, 0.0f);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<translucent>(1.5, 0.0f);
    world.add(std::make_shared<sphere>(vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<diffuse>(vec3(0.4, 0.2, 0.1));
    world.add(std::make_shared<sphere>(vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(vec3(4, 1, 0), 1.0, material3));

    return world;
}


int main()
{
    scene world = random_scene();
    
    renderer_desc_t r_description;
    r_description.WIDTH = 720;
    r_description.ASPECT_RATIO = static_cast < double>(16.0 / 9.0);
    r_description.CAMERA_LOOK_FROM = vec3(13, 2, 3);
    r_description.CAMERA_LOOK_AT = vec3(0, 0, 0);
    r_description.CAMERA_VERTICAL_FOV = 20;
    r_description.RAY_BOUNCE_LIMIT = 10;
    r_description.RAY_SAMPLES_PER_PIXEL = 300;

    renderer path_tracer{ r_description , world };
    path_tracer.render();

    std::ofstream outfile;
    outfile.open("output.ppm", std::ios::out | std::ios::trunc);
    path_tracer.getImage()->output_ppm(outfile);
    outfile.close();

    Graphics gfx{ path_tracer.getImage()->width, path_tracer.getImage()->height };
    gfx.init();
    gfx.error_check();


    while (!gfx.livestatus())
    {
        gfx.startFrame();

        gfx.bindScreenQuad();
        gfx.bindTexture((path_tracer.getImage()->pixel_data.address()), path_tracer.getImage()->width, path_tracer.getImage()->height);
        gfx.draw();
        
        gfx.endFrame();
    }

	return 0;

}
