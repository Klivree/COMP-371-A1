#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

class Light {
public:
	Light(glm::vec3 lightPOS, GLfloat lightFOV, GLfloat lightNearPlane, GLfloat lightFarPlane, glm::vec3 lightFocus, int depthMapTextureSize);

	void createMatrices(GLuint& shaderProgram);


	glm::vec3 POS;
	glm::vec3 orientation;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	GLfloat nearPlane;
	GLfloat farPlane;
	GLfloat FOV;
	int DEPTH_MAP_TEXTURE_SIZE;
};







#endif