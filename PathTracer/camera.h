#pragma once
#include "util.h"

class camera {
private:
    float projection_screen_height;
    float projection_screen_width;
    float focal_length;

    vec3 camera_origin;
    vec3 horizontal;
    vec3 vertical;

    vec3 pt_lower_left_corner;

    double samples_per_pixel;   
public:
    camera() = default;

    camera(vec3 lookfrom, vec3 lookat, vec3 view_up, double vertical_fov, double aspectratio, double spp) {
        //Viewport defs. Distance from camera to screen (say we have a virtual screen and we project rays to all points on it,Thats what a viewport is.
        double theta = degrees_to_radians(vertical_fov);
        double h = tan(theta / 2);

        projection_screen_height = 2.0 *h;
        projection_screen_width = aspectratio * projection_screen_height;


        //orthonormal basis for the entire camera.
        vec3 w = unit_vector(lookfrom - lookat); //replacement for focal length, essentially the axis between lookfrom and lookat, line joining camera and screen centre essentially(so focal len)

        vec3 u = unit_vector(cross(view_up, w));
        vec3 v = cross(w, u);

        //Camera defs
        camera_origin = lookfrom;
        horizontal = projection_screen_width * u;
        vertical = projection_screen_height * v;

        //lower left corner point of screen, our reference point on the screen, on top of which we will traverse all pixels.
        pt_lower_left_corner = camera_origin - (horizontal / 2) - (vertical / 2) - w;
        //Transform an origin point , to a point on screen lower left corner. Horizontal contributes x coord, Vertical y coord, and length the z. To imagine,
        //go through the seqeunce, taek origin, negate and go to left half of horixontal, then with vertical and so on, we will get to lowerleft.
        samples_per_pixel = spp;
    }

    int getspp() { return samples_per_pixel; }

    ray get_ray_to_screen(double xpercent, double ypercent) {
        return ray(camera_origin, pt_lower_left_corner + xpercent * horizontal + ypercent * vertical - camera_origin);
        //Create ray (from origin cam origin)using reference point and adding the xpercent 
        //and y percent traversed by ray pointer (scan lining, all pixels from lowerleftcorner)
    }
};