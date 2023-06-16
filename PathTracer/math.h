#pragma once
#include <iostream>
#include <vector>
#include <random>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline double random_double(double min, double max) {
	// Returns a random real in [min,max).
	return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}


class vec3 {
public:
	double x;
	double y;
	double z;

	vec3(float a = 0.0f, float b = 0.0f, float c = 0.0f) : x{ a }, y{ b }, z{ c } {}

	//If I am passing around stuff by const ref, all operators must be const, 
	//they should be const anyway.

	vec3 operator-() const {
		return vec3(-x, -y, -z);
	}

	double getx() const { return x; }
	double gety() const { return y; }
	double getz() const { return z; }

	double length() const {
		return sqrt(x * x + y * y + z * z);
	}

	double length_squared() {
		return length() * length();
	}

	double operator[](int index) {
		if (index == 0) { return x; }
		else if (index == 1) { return y; }
		else { return z; }
	}

	inline static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	inline static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	bool near_zero() const {
		const auto s = 1e-8;
		return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s);
	}

};

inline vec3 operator+(const vec3& first, const vec3& second) {
	return vec3(first.x + second.x, first.y + second.y, first.z + second.z);
}

inline vec3 operator-(const vec3& first, const vec3& second) {
	return vec3(first.x - second.x, first.y - second.y, first.z - second.z);
}

inline vec3 operator*(const vec3& vec, float scalar) {
	return vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

inline vec3 operator*(const vec3& first, const vec3& second) {
	return vec3(first.x * second.x, first.y * second.y, first.z * second.z);
}

inline vec3 operator*(float scalar, const vec3& vec) {
	return vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

inline vec3 operator/(const vec3& vec, float scalar) {
	return vec3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}

inline double dot(const vec3& first, const vec3& second) {
	return (first.x * second.x +
		first.y * second.y +
		first.z * second.z);
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x);
}

inline std::ostream& operator<<(std::ostream& out, const vec3& vec) {
	out << vec.x << ", " << vec.y << ", " << vec.z;
	return out;
}

inline bool operator==(const vec3 first, const vec3& second) {
	if (first.x == second.x && first.y == second.y && first.z == second.z) {
		return true;
	}
	else {
		return false;
	}
}

inline vec3 unit_vector(const vec3& v) {
	return v / v.length();
}

class ray {
private:
	vec3 origin;
	vec3 direction;
public:
	//vec3 is a complex data type, pass by reference is faster
	ray(const vec3& org = vec3(0.0f), const vec3& dir = vec3(0.0f)) : origin{org}, direction{dir} {}
	//default constructor and destructor

	vec3 getorigin() const { return origin; }
	vec3 getdirection() const { return direction; }

	//A + Bt is the formula for a line in space, so at param t? thats point on line at t.
	vec3 pointat(double t) const { return origin + direction * t; }
};

vec3 random_in_unit_sphere() {
	//Monte carlo rejection method, keep random sampling random vectors,
	//choose first vector that lands inside a sphere (satisfies sphere eqn)
	while (true) {
		vec3 rand = vec3::random(-1, 1); //Find random points in a unit cube of -1 to 1
		if ((rand.length() * rand.length()) >= 1) continue;
		return rand;
	}
}
//This above bounce ray selection function can find any POINT INSIDE the sphere, and can give inaccurate results as points inside spehere
//tend to be close to normal and grazing angles arent sampled much

vec3 random_unit_vector() {
	return unit_vector(random_in_unit_sphere());
}
//This fun generates (simple normalization) points ON THE SPHERE, so less clumping and more uniform point distribuiton.
//Both of these changes are due to the more uniform scattering of the light rays, fewer rays are scattering toward the normal

vec3 random_in_hemisphere(const vec3& normal) {
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}
//More intuitive approach, no need to bring our bounce ray sphere on top of intersection point(trasnlate by normal), simply
//make a hemisphere with point of intersectiion as the center, and find random points on the hemishphere! 

vec3 reflect(const vec3& invector, const vec3& normal) {
	return invector - 2 * dot(invector, normal) * normal; //B is the ortho of normal on to ray direction
}

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	auto cos_theta = fmin(dot(-uv, n), 1.0);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}