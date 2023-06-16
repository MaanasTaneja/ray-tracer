#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <memory>
#include "math.h"

template<typename T>
class pointer {
private:
	T* raw;
public:
	//Follow rule of three, needed to make construcor, so need to now make destrucotr and copy constrcutor also
	pointer(T* ptr = nullptr) : raw{ ptr } {}
	pointer(const pointer& copy) {
		raw = new T;
		*raw = *(copy.raw); //Standard copy assignment of T.
	}

	//We implement rule of 5 also
	pointer(const pointer&& move) {
		raw = move.raw;
		move.raw = nullptr;
		//move pointer to different pointer, no need to copy and set other to nullptr
	}

	//Alwasy return references, because REFERENCES != POINTERS , why? because once references are bound they cannot be changed and they are essentially like normal objetcs?
	//Also if i do return this, it will copy construct another pointer and reosurce will be duplictaed! Thats why returning a reference in = chain is better!

	pointer& operator=(const pointer& other) {
		if (&other == this) {
			return *this;
		}

		delete raw;
		raw = new T;  
		*raw = *(other.raw);

		return *this;
		//This is currently calling object and seeing as this is a member function overload, then a = b, this is a.
	}
	
	//Can't be const as we must modify 
	pointer& operator=(pointer&& other) {
		if (&other == this) {
			return *this;
		}

		delete raw;
		raw = other.raw;
		other.raw = nullptr;

		return *this;
	}

	inline T& operator*() {
		return *raw;
	}

	inline T* operator->() {
		return raw;
	}

	~pointer() {
		delete raw;
	}
};

template<typename T>
class dyn_array {
private:
	T* array;
	size_t size;
	int pointer;
public:
	dyn_array(size_t s = 0) :size{ s }, array{ nullptr } {
		array = new T[s];
	}
	~dyn_array() {
		delete array;
		array = nullptr;
	}

	dyn_array(const dyn_array& copy) {
		delete array;
		array = new T;
		*array = *(copy.array);
		size = copy.size;
	}
	dyn_array(dyn_array&& move) {
		array = move.array;
		size = move.size;

		move.array = nullptr;
	}

	dyn_array& operator=(const dyn_array& copy) {
		if (&copy == this) {
			return *this; //Return object itself (by value which will be referenced by return type)
		}

		delete array;
		array = new T;
		*array = *(copy.array);
		size = copy.size;

		return *this;  //Returned altered self.
	}

	dyn_array& operator=(dyn_array&& move) noexcept{
		if (&move == this) {
			return *this; //Return object itself (by value which will be referenced by return type)
		}

		delete array;
		array = (move.array);
		size = move.size;

		move.array = nullptr;

		return *this;  //Returned altered self.
	}

	T& operator[](int index) {
		return array[index];
	}

	T* address() {
		return &array[0];
	}

	void push_back(T element) {
		array[pointer] = element;
		pointer++;
	}

	void pop() {
		pointer--;
	}

	size_t getsize() { return size; }
};

class Image {
public:
	dyn_array<float> pixel_data;
	int16_t width;
	int16_t height;
	double aspect_ratio;

	Image(int16_t w, double aspectratio) : width{ w }, aspect_ratio{ aspectratio } {
		height = static_cast<int>(w / aspect_ratio);
		pixel_data = dyn_array<float>(width * height * 3);
	}
	Image(int16_t w, int16_t h) : width{ w }, height{ h } {
		aspect_ratio = static_cast<double>(w / h);
		pixel_data = dyn_array<float>(width * height * 3);
	}
	~Image() = default;
	int16_t getwidth() { return width; }
	int16_t getheight() { return height; }
	double getaspectratio() { return aspect_ratio; }

	void write_color_sampled(vec3 color, int spp) {
		double r = color.getx();
		double g = color.gety();
		double b = color.getz();

		// Divide the color by the number of samples.
		double scale = 1.0 / spp;
		r = std::sqrt(scale * r);
		g = std::sqrt(scale * g);
		b = std::sqrt(scale * b);

		pixel_data.push_back(static_cast<float>(r));
		pixel_data.push_back(static_cast<float>(g));
		pixel_data.push_back(static_cast<float>(b));
	}

	void output_ppm(std::ostream& out) {
		out << "P3\n" << width << ' ' << height << "\n255\n";
		for (int i = 0; i < pixel_data.getsize(); i++) {
			if (i % 2 == 0) {
				out << static_cast<int>(256 * clamp(pixel_data[i], 0.0, 0.9999)) << '\n';
			}
			else {
				out << static_cast<int>(256 * clamp(pixel_data[i], 0.0, 0.9999)) << ' ';
			}

		}
	}
};


class Image_deprecated {
public:
	std::vector<vec3> pixel_data;
	int16_t width;
	int16_t height;
	double aspect_ratio;
	//Vector handls its copying/moving by itself 

	Image_deprecated(int16_t w, double aspectratio) : width{w}, aspect_ratio{ aspectratio } {
		height = static_cast<int>(w / aspect_ratio);
		pixel_data.reserve(width * height);  //Should decrease overhead, as we would not be resizing the vector every time we add a pixel to the list.
		//When performing a large number of push_back() calls, remember to call vector::reserve(), push back when out of mem, thus having to reallocate is O(N), increases with increase
		//in vector size, where as simple push back when space available is O(1).
	}
	Image_deprecated(int16_t w, int16_t h) : width{ w }, height{ h } {
		aspect_ratio = static_cast<double>(w / h);
		pixel_data.reserve(width * height);
	}
	~Image_deprecated() = default;
	int16_t getwidth() { return width; }
	int16_t getheight() { return height; }
	double getaspectratio() { return aspect_ratio; }

	void write_color_sampled(vec3 color, int spp) { 
		double r = color.getx();
		double g = color.gety();
		double b = color.getz();

		// Divide the color by the number of samples.
		double scale = 1.0 / spp;
		r *= scale;
		g *= scale;
		b *= scale;

		pixel_data.push_back(vec3(r,g,b));
	}

	void write_color(vec3 color) {
		pixel_data.push_back(color);
	}

	void output_ppm(std::ostream& out) {
		out << "P3\n" << width << ' ' << height << "\n255\n";
		for (const vec3& pixel : pixel_data) {
			out << static_cast<int>(256 * clamp(pixel.getx(), 0.0, 0.9999)) << ' '
				<< static_cast<int>(256 * clamp(pixel.gety(), 0.0, 0.9999)) << ' '
				<< static_cast<int>(256 * clamp(pixel.getz(), 0.0, 0.9999)) << '\n';
		}
	}
};


