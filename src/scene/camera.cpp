
#include <iostream>

// glm
#include <glm/gtc/matrix_transform.hpp>

// project
#include "camera.hpp"
#include "opengl.hpp"


using namespace std;
using namespace glm;


void Camera::setPositionOrientation(const vec3 &pos, float yaw, float pitch) {
	m_position = pos;
	m_yaw = yaw;
	m_pitch = pitch;

	// update rotation matrix (based on yaw and pitch)
	m_rotation = rotate(mat4(1), m_yaw, vec3(0, 1, 0)) * rotate(mat4(1), m_pitch, vec3(1, 0, 0));
}


Ray Camera::generateRay(const vec2 &pixel) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Generate a ray in the scene using the camera position,
	// rotation field of view on the y axis (fovy) and the image
	// size. The pixel is given in image coordinates [0, imagesize]
	// This COULD be done by first creating the ray in ViewSpace
	// then transforming it by the position and rotation to get
	// it into worldspace.
	//-------------------------------------------------------------
	

	Ray ray;

	// YOUR CODE GOES HERE
	// ...
	
	vec2 ndclol = vec2((2 + 0.5) / 6, (6 + 0.5) / 3.38);
	float ass = 16 / 9;
	vec2 ps = vec2(2 * ndclol.x-1 ,2*ndclol.y-1 );
	vec2 pc = vec2(2 * ps.x * ass*tan(1.f/2), 1 - 2 * ps.y*tan(1.f / 2));

	//cout << (pc).x << "--" << pc.y << "\n";

	vec2 ndc = vec2(2) * pixel / (m_image_size)-vec2(1);

	float ar = m_image_size.x / m_image_size.y;

	float Py = tan(m_fovy / 2);
	float Px = ar * tan(m_fovy / 2);

	vec3 pos_on_image_plane = vec3(ndc * vec2(Px, Py), -1);

	ray = Ray(m_position, pos_on_image_plane);

	return ray;
}
