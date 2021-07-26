#include "Light.hpp"

Light::Light(glm::vec3 lightPOS, GLfloat lightFOV, GLfloat lightNearPlane, GLfloat lightFarPlane, glm::vec3 lightFocus, int depthMapTextureSize) {
	POS = lightPOS;
	FOV = lightFOV;
	nearPlane = lightNearPlane;
	farPlane = lightFarPlane;
	orientation = lightFocus;
}

void Light::createMatrices(GLuint& shaderProgram) {
	//creating matrices
	glm::mat4 lightProjMatrix = glm::perspective(FOV, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, nearPlane, farPlane);
	glm::mat4 lightViewMatrix = glm::lookAt(POS, orientation, up);

	// sending the view and projection matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, &lightViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, GL_FALSE, &lightProjMatrix[0][0]);
}
