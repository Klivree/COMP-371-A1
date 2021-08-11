#include "PointLight.hpp"
#include <vector>
#include <string>

PointLight::PointLight(glm::vec3 lightPOS, GLfloat lightFarPlane, GLfloat lightConstantTerm, GLfloat lightLinearTerm, GLfloat lightQuadTerm, glm::vec3 lightColor, int depthMapTextureSize){
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
	nearPlane = 0.1f;
	farPlane = lightFarPlane;
	DEPTH_MAP_TEXTURE_SIZE = depthMapTextureSize;
	color = lightColor;

	constTerm = lightConstantTerm;
	linearTerm = lightLinearTerm;
	QuadTerm = lightQuadTerm;

	
}

void PointLight::updateShadowShader(GLuint& shaderProgram) {
	// sending the view and projection matrices to the vertex shader

	glm::mat4 lightProjMatrix = glm::perspective(glm::radians(90.0f), (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, nearPlane, farPlane);
	
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(lightProjMatrix * glm::lookAt(POS, POS + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	
	for (int i = 0; i < 6; ++i) {
		std::string uniformName = "shadowMatrices[" + std::to_string(i) + "]";
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, uniformName.c_str()), 1, GL_FALSE, &shadowTransforms[i][0][0]);
	}

	glUniform1f(glGetUniformLocation(shaderProgram, "lightFarPlane"), farPlane);
	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPosition"), 1, &POS[0]);

}


void PointLight::updateSceneShader(GLuint& shaderProgram, std::string lightName) { updateSceneShader(shaderProgram, lightName, false); }

void PointLight::updateSceneShader(GLuint& shaderProgram, std::string lightName, bool enableShadows) {
	////////////////	FRAGMENT SHADER	///////////////
	glUniform3fv(glGetUniformLocation(shaderProgram, &(lightName + ".lightColor")[0]), 1, &color[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, &(lightName + ".POS")[0]), 1, &POS[0]);

	glUniform1f(glGetUniformLocation(shaderProgram, &(lightName + ".lightConstTerm")[0]), constTerm);
	glUniform1f(glGetUniformLocation(shaderProgram, &(lightName + ".lightLinearTerm")[0]), linearTerm);
	glUniform1f(glGetUniformLocation(shaderProgram, &(lightName + ".lightQuadTerm")[0]), QuadTerm);
	glUniform1f(glGetUniformLocation(shaderProgram, &(lightName + ".lightFarPlane")[0]), farPlane);

	glUniform1i(glGetUniformLocation(shaderProgram, "enableShadows"), enableShadows);
}