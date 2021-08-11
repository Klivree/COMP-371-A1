#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <string>

class PointLight {
public:
	PointLight(glm::vec3 lightPOS, GLfloat lightFarPlane, GLfloat lightConstantTerm, GLfloat lightLinearTerm, GLfloat lightQuadTerm, glm::vec3 lightColor, int depthMapTextureSize);

	void updateShadowShader(GLuint& shaderProgram);

	void updateSceneShader(GLuint& shaderProgram, std::string lightName);

	void updateSceneShader(GLuint& shaderProgram, std::string lightName, bool enableShadows);


	glm::vec3 POS;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 color;
	GLfloat constTerm, linearTerm, QuadTerm, nearPlane, farPlane;

	int DEPTH_MAP_TEXTURE_SIZE;
	
};







#endif