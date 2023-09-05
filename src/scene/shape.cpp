
// std
#include <algorithm>
#include <utility>
#include <iostream>


// glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// project
#include "shape.hpp"


using namespace glm;
using namespace std;

RayIntersection AABB::intersect(const Ray &ray) {
	RayIntersection intersect;
	vec3 rel_origin = ray.origin - m_center;

	// start magic
	// x
	float rdx_inv = 1 / ray.direction.x;
	float tx1 = (-m_halfsize.x - rel_origin.x) * rdx_inv;
	float tx2 = (m_halfsize.x - rel_origin.x) * rdx_inv;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	// y
	float rdy_inv = 1 / ray.direction.y;
	float ty1 = (-m_halfsize.y - rel_origin.y) * rdy_inv;
	float ty2 = (m_halfsize.y - rel_origin.y) * rdy_inv;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	// z
	float rdz_inv = 1 / ray.direction.z;
	float tz1 = (-m_halfsize.z - rel_origin.z) * rdz_inv;
	float tz2 = (m_halfsize.z - rel_origin.z) * rdz_inv;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	if (tmax < tmin) return intersect;
	// end magic

	intersect.m_distance = tmin < 0 ? tmax : tmin;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = tmax >= 0;
	vec3 work_out_a_name_for_it_later = abs((intersect.m_position - m_center) / m_halfsize);
	float max_v = std::max(work_out_a_name_for_it_later[0], std::max(work_out_a_name_for_it_later[1], work_out_a_name_for_it_later[2]));
	intersect.m_normal = normalize(mix(intersect.m_position - m_center, vec3(0), lessThan(work_out_a_name_for_it_later, vec3(max_v))));
	intersect.m_uv_coord = (abs(intersect.m_normal.x) > 0) ?
		vec2(intersect.m_position.y, intersect.m_position.z) :
		vec2(intersect.m_position.x, intersect.m_position.y + intersect.m_position.z);
	intersect.m_shape = this;

	return intersect;
}


RayIntersection Sphere::intersect(const Ray &ray) {
	RayIntersection intersect;
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement the intersection method for Sphere that returns
	// a RayIntersection object with valid == false if the 
	// ray doesn't intersect and true otherwise. If true, then
	// remember to fill out each feild in the object correctly:
	// - m_valid : true if object is itersected
	// - m_distance : distance along the ray of the intersection
	// - m_position : position on the surface of the intersection
	// - m_normal : normal on the surface of the intersection
	// - m_shape : set to "this"
	// - m_uv_coord : texture coordinates (challenge only)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	vec3 rel_origin = ray.origin - m_center;


	float a = dot(ray.direction, ray.direction);
	float b = 2.0 * dot(rel_origin, ray.direction);
	float c = dot(rel_origin, rel_origin) - m_radius * m_radius;


	float dis = b * b - 4 * a * c;


	// start magic
	if (dis < 0) {
		return intersect;
	}

	float t1 = ((-b - sqrt(dis)) / (2 * a));
	float t2 = ((-b + sqrt(dis)) / (2 * a));
	float t_min = std::min(t1,t2);

	if (t_min < 0) {
		return intersect;
	}
	
	intersect.m_distance = t_min;
	intersect.m_position = ray.origin + t_min * ray.direction;
	intersect.m_valid = true;
	intersect.m_normal = normalize(intersect.m_position - m_center);
	intersect.m_shape = this;
	intersect.m_uv_coord = vec2(0, 0);

	return intersect;
}


// Intersect for Plane
RayIntersection Plane::intersect(const Ray& ray) {
	RayIntersection intersect;
	// YOUR CODE GOES HERE
	// ...
	vec3 o = ray.origin;
	vec3 d = ray.direction;
	vec3 n = m_normal;

	float t = dot((m_point - o) , n) / dot(d , n);

	//float t = t3.x;

	if (t < 0) {
		return intersect;
	}

	intersect.m_distance = t;
	intersect.m_position = o+ t*d;
	intersect.m_normal = n;

	intersect.m_valid = true;
	intersect.m_shape = this;

	return intersect;
}

// Intersect for Disk
RayIntersection Disk::intersect(const Ray& ray) {
	RayIntersection intersect;
	// YOUR CODE GOES HERE
	// ...
	vec3 o = ray.origin;
	vec3 d = ray.direction;
	vec3 n = m_normal;

	float t = dot((m_point - o), n) / dot(d, n);

	//float t = t3.x;

	if (t < 0) {
		return intersect;
	}

	vec3 position = o + t * d;
	vec3 rel_point = position- m_point;
	float len = length(rel_point);

	if (len > m_radius) {
		return intersect;
	}


	intersect.m_distance = t;
	intersect.m_position = o + t * d;
	intersect.m_normal = n;

	intersect.m_valid = true;
	intersect.m_shape = this;

	return intersect;
}

// Intersect for Triangle
RayIntersection Triangle::intersect(const Ray& ray) {
	RayIntersection intersect;
	// YOUR CODE GOES HERE
	// ...
	vec3 o = ray.origin;
	vec3 d = ray.direction;

	vec3 d0 = (m_v1 - m_v0);
	vec3 d1 = (m_v2 - m_v0);

	

	vec3 n = cross((m_v1 - m_v0), (m_v2 - m_v0));
	
	float t = dot((m_v0 - o), n) / dot(d, n);
	if (t < 0) {
		return intersect;
	}
	
	vec3 position = o + t * d;


	vec3 f0 = cross((m_v1 - m_v0),(position - m_v0));
	vec3 f1 = cross((m_v2 - m_v1), (position - m_v1));
	vec3 f2 = cross((m_v0 - m_v2), (position - m_v2));


	if (dot(n,f0) > 0 && dot(n, f1) > 0 && dot(n, f2) > 0) {
		intersect.m_valid = true;
	}
	else {
		return intersect;
	}

	intersect.m_distance = t;
	intersect.m_position = o + t * d;
	intersect.m_normal = -n;

	intersect.m_shape = this;


	return intersect;
}

