
// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

// std
#include <random>

// project
#include "scene.hpp"
#include "shape.hpp"
#include "light.hpp"
#include "material.hpp"
#include "path_tracer.hpp"


using namespace std;
using namespace glm;


vec3 SimplePathTracer::sampleRay(const Ray &ray, int) {
	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {

		// simple grey shape shading
		float f = abs(dot(-ray.direction, intersect.m_normal));
		vec3 grey(0.5, 0.5, 0.5);
		return mix(grey / 2.0f, grey, f);
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 CorePathTracer::sampleRay(const Ray &ray, int) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the ambient, diffuse
	// and specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays). You do
	// not need to use the depth argument for this implementation.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	float ambientStrength = 0.1;
	float diffuseStrength = 0.4;
	float specularStrength = 0.8;


	vec3 color = vec3(0);
	RayIntersection intersect = m_scene->intersect(ray);


	//if ray hit something
	if (intersect.m_valid) {

		for (shared_ptr<Light> light: m_scene->lights()) {
			//increment the color by the materials diffuse element
			color += light->ambience() * intersect.m_material->diffuse();

			//check if the light is occluded
			if (!light->occluded(m_scene, intersect.m_position)) {

				// diffuse
				float diff = std::max(0.0f, dot(intersect.m_normal, normalize(-light->incidentDirection(intersect.m_position))));
				vec3 diffuse = light->irradiance(intersect.m_position) * diff * intersect.m_material->diffuse();

				// specular
				vec3 reflects = reflect(light->incidentDirection(intersect.m_position), intersect.m_normal);
				float spec = pow(std::max(0.0f, dot(normalize(reflects), normalize(-ray.direction))), intersect.m_material->shininess());
				vec3 specular = light->irradiance(intersect.m_position)* spec * intersect.m_material->specular();

				color += diffuse*diffuseStrength + specular * specularStrength;
			}
		}
		return color;
	}
	
	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 CompletionPathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Using the same requirements for the CorePathTracer add in 
	// a recursive element to calculate perfect specular reflection.
	// That is compute the reflection ray off your intersection and
	// sample a ray in that direction, using the result to additionally
	// light your object. To make this more realistic you may weight
	// the incoming light by the (1 - (1/shininess)).
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	float ambientStrength = 0.1;
	float diffuseStrength = 0.4;
	float specularStrength = 0.8;


	vec3 color = { 0.0f, 0.0f, 0.0f };
	vec3 reflected_color = { 0.0f, 0.0f, 0.0f };
	RayIntersection intersect = m_scene->intersect(ray);


	//if ray hit something
	if (intersect.m_valid) {

		for (shared_ptr<Light> light : m_scene->lights()) {
			//increment the color by the materials diffuse element
			color += light->ambience() * intersect.m_material->diffuse();

			//check if the light is occluded
			if (!light->occluded(m_scene, intersect.m_position)) {
				
				// diffuse
				float diff = std::max(0.0f, dot(intersect.m_normal, normalize(-light->incidentDirection(intersect.m_position))));
				vec3 diffuse = light->irradiance(intersect.m_position) * diff * intersect.m_material->diffuse();

				// specular
				vec3 reflects = reflect(light->incidentDirection(intersect.m_position), intersect.m_normal);
				float spec = pow(std::max(0.0f, dot(normalize(reflects), normalize(-ray.direction))), (intersect.m_material->shininess()));
				vec3 specular = light->irradiance(intersect.m_position) * spec * intersect.m_material->specular();

				color += diffuse * diffuseStrength + specular * specularStrength;
				
				
				
			}
			
			

		}
		if (depth > 1) {

			vec3 reflects = reflect(ray.direction, intersect.m_normal);
			Ray reflectRay = Ray(intersect.m_position, reflects);
			color += CompletionPathTracer::sampleRay(reflectRay, depth - 1) * (1 - (1 / intersect.m_material->shininess())) * intersect.m_material->specular();

		}
		return color;
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 ChallengePathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the diffuse and 
	// specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays).
	// Implement support for textured materials (using a texture
	// for the diffuse portion of the material).
	//
	// EXTRA FOR EXPERTS :
	// Additionally implement indirect diffuse and specular instead
	// of using the ambient lighting term.
	// The diffuse is sampled from the surface hemisphere and the
	// specular is sampled from a cone of the phong lobe (which
	// gives a glossy look). For best results you need to normalize
	// the lighting (see http://www.thetenthplanet.de/archives/255)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	glm::vec3 eye = glm::vec3(0, 0, 0);
	glm::vec3 screenCenter = glm::vec3(0, 0, -1);
	glm::vec3 screenUp = glm::vec3(0, 1, 0);
	glm::vec3 screenRight = glm::vec3(1, 0, 0);
	glm::vec3 screenNormal = normalize(cross(screenRight, screenUp));
	glm::vec3 screenPixelCenter = screenCenter + screenRight * 3.0f + screenUp * -3.0f;
	glm::vec3 screenPixelDirection = normalize(screenPixelCenter - eye);

	if (depth <= 0) {
        return vec3(0.0f);
    }

	RayIntersection intersect = m_scene->intersect(ray);

	/*if (intersect.m_valid) {

		vec3 ambient = calculateAmbient(intersect.m_material);
		vec3 diffuse = calculateDiffuse(intersect.m_material, intersect.m_normal, m_scene->lights());
		vec3 specular = calculateSpecular(intersect.m_material, intersect.m_normal, m_scene->lights(), ray.direction);




		return ambient + diffuse + specular;
	}*/


	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}

//glm::vec3 ChallengePathTracer::calculateAmbient(const Material& material) {
//	// You can use the ambient reflection coefficient of the material
//	// to control the intensity of the ambient lighting.
//	glm::vec3 ambientColor = material.ambientColor; // This should be part of your Material struct.
//
//	// You can also multiply the ambient color by a global ambient intensity.
//	glm::vec3 globalAmbientColor = glm::vec3(0.1f); // Adjust the intensity as needed.
//
//	return ambientColor * globalAmbientColor;
//}
//
//glm::vec3 ChallengePathTracer::calculateDiffuse(const Material& material, const glm::vec3& normal, const std::vector<Light>& lights) {
//	glm::vec3 diffuseColor = glm::vec3(0.0f);
//
//	for (const Light& light : lights) {
//		// Calculate the light direction from the intersection point to the light source.
//		glm::vec3 lightDirection = glm::normalize(light.position - intersectionPoint); // Calculate the intersection point.
//
//		// Calculate the cosine of the angle between the light direction and the surface normal.
//		float cosTheta = glm::dot(normal, lightDirection);
//
//		// Ensure that the cosine is positive (the light is on the correct side of the surface).
//		if (cosTheta > 0.0f) {
//			// Compute the diffuse reflection for this light source.
//			glm::vec3 lightIntensity = light.intensity; // Adjust the light intensity as needed.
//			diffuseColor += material.diffuseColor * lightIntensity * cosTheta;
//		}
//	}
//
//	return diffuseColor;
//}
//
//glm::vec3 ChallengePathTracer::calculateSpecular(const Material& material, const glm::vec3& normal, const std::vector<Light>& lights, const glm::vec3& viewDirection) {
//	glm::vec3 specularColor = glm::vec3(0.0f);
//
//	for (const Light& light : lights) {
//		// Calculate the light direction from the intersection point to the light source.
//		glm::vec3 lightDirection = glm::normalize(light.position - intersectionPoint); // Calculate the intersection point.
//
//		// Calculate the reflection direction (normalized) of the light ray.
//		glm::vec3 reflectionDirection = glm::reflect(-lightDirection, normal);
//
//		// Calculate the cosine of the angle between the reflection direction and the view direction.
//		float cosAlpha = glm::max(glm::dot(reflectionDirection, viewDirection), 0.0f);
//
//		// Compute the specular reflection for this light source.
//		glm::vec3 lightIntensity = light.intensity; // Adjust the light intensity as needed.
//		glm::vec3 specularTerm = material.specularColor * lightIntensity * pow(cosAlpha, material.specularExponent);
//
//		specularColor += specularTerm;
//	}
//
//	return specularColor;
//}