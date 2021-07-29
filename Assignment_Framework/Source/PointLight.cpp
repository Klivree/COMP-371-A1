#include "PointLight.hpp"
#include <vector>

PointLight::PointLight(glm::vec3 lightPOS, GLfloat lightFOV, GLfloat lightConstantTerm, GLfloat lightLinearTerm, GLfloat lightQuadTerm, glm::vec3 lightFocus, glm::vec3 lightColor, int depthMapTextureSize){
	/* Contructor for the PointLight object
	*		lightPOS - position of the light
	*		lightFOV - field of view of the light in degrees
	*		lightConstantTerm - constant term of the attenuation formula
	*		lightLinearTerm - linear term of the attenuation formula
	*		lightQuadTerm - quadratic term of the attenuation formula
	*		lightFocus - direction of the point light
	*		lightColor - color of the light
	*		depthMapTextureSize - size of the light's depth map
	*/ 
	POS = lightPOS;
	FOV = lightFOV;
	nearPlane = 1.0f;
	farPlane = 100.0f;
	orientation = lightFocus;
	DEPTH_MAP_TEXTURE_SIZE = depthMapTextureSize;
	color = lightColor;

	constTerm = lightConstantTerm;
	linearTerm = lightLinearTerm;
	QuadTerm = lightQuadTerm;

	
}

void PointLight::updateShadowShader(GLuint& shaderProgram) {
	// sending the view and projection matrices to the vertex shader

	glm::mat4 lightProjMatrix = glm::perspective(glm::radians(90.0f), (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, nearPlane, farPlane);
	

	// values for all 6 sides of the point light
	glm::mat4 shadowTransform1 = lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	glm::mat4 shadowTransform2 = lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	glm::mat4 shadowTransform3 = lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 shadowTransform4 = lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
	glm::mat4 shadowTransform5 = lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
	glm::mat4 shadowTransform6 = lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
	
	// updating the values to the shadow geometry shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "shadowMatrices1"), 1, GL_FALSE, &shadowTransform1[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "shadowMatrices2"), 1, GL_FALSE, &shadowTransform2[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "shadowMatrices3"), 1, GL_FALSE, &shadowTransform3[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "shadowMatrices4"), 1, GL_FALSE, &shadowTransform4[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "shadowMatrices5"), 1, GL_FALSE, &shadowTransform5[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "shadowMatrices6"), 1, GL_FALSE, &shadowTransform6[0][0]);

}


void PointLight::updateSceneShader(GLuint& shaderProgram) {
	////////////////	VERTEX SHADER	////////////////
	// sending the view and projection matrices to the vertex shader
	glm::mat4 lightProjMatrix = glm::perspective(glm::radians(90.0f), (float) (768 / 1024), nearPlane, farPlane);
	glm::mat4 lightViewMatrix = glm::lookAt(POS, orientation, up);
	glm::mat4 lightSpaceMatrix = lightProjMatrix * lightViewMatrix;

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	////////////////	FRAGMENT SHADER	////////////////
	glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, &color[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPosition"), 1, &POS[0]);

	glUniform1f(glGetUniformLocation(shaderProgram, "lightConstTerm"), constTerm);
	glUniform1f(glGetUniformLocation(shaderProgram, "lightLinearTerm"), linearTerm);
	glUniform1f(glGetUniformLocation(shaderProgram, "lightQuadTerm"), QuadTerm);
	glUniform1f(glGetUniformLocation(shaderProgram, "lightFarPlane"), farPlane);

}