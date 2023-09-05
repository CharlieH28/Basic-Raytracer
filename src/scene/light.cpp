
// glm
#include <glm/gtc/constants.hpp>

// project
#include "light.hpp"

using namespace glm;
# define M_PI           3.14159265358979323846  /* pi */

bool DirectionalLight::occluded(Scene *scene, const vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Determine whether the given point is being occluded from
	// this directional light by an object in the scene.
	// Remember that directional lights are "infinitely" far away
	// so any object in the way would cause an occlusion.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	//Ray shadowRay = Ray(point, -m_direction);
	vec3 direction = -incidentDirection(point);

	Ray shadowRay = Ray(point, direction);

	RayIntersection intersect = scene->intersect(shadowRay);

	if (intersect.m_valid) {

		return true;
	}
	return false;
}


vec3 DirectionalLight::incidentDirection(const vec3 &) const {
	return m_direction;
}


vec3 DirectionalLight::irradiance(const vec3 &) const {
	return m_irradiance;
}


bool PointLight::occluded(Scene *scene, const vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Determine whether the given point is being occluded from
	// this directional light by an object in the scene.
	// Remember that point lights are somewhere in the scene and
	// an occulsion has to occur somewhere between the light and 
	// the given point.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	vec3 light_dir = -incidentDirection(point);
	float light_length = length(light_dir);
	if(light_length > 0){
		light_dir = normalize(light_dir);
	}

	Ray ray = Ray(point, light_dir);
	RayIntersection intersect = scene->intersect(ray);
	

	if (intersect.m_valid && intersect.m_distance < light_length) {

		return true;
	}

	return false;
}


vec3 PointLight::incidentDirection(const vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Return the direction of the incoming light (light to point)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	

	return (point - m_position);
}


vec3 PointLight::irradiance(const vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Return the total irradiance on the given point.
	// This requires you to convert the flux of the light into
	// irradiance by dividing it by the surface of the sphere
	// it illuminates. Remember that the surface area increases
	// as the sphere gets bigger, ie. the point is further away.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	vec3 light_dir = -incidentDirection(point);
	float light_length = length(light_dir);
	float light_length_sqr = light_length * light_length;
	vec3 m_irradiance = m_flux / vec3(4 * M_PI + light_length_sqr);
	return m_irradiance;
}
