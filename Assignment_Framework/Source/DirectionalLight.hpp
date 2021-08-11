#ifndef DIRECTIONAL_LIGHT_CLASS
#define DIRECTIONAL_LIGHT_CLASS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <string>

using namespace std;
using namespace glm;

class DirectionalLight {
public:
	DirectionalLight(vec3 pPOS, vec3 pDirection) {
		direction = pDirection;
		POS = pPOS;
		color = vec3(1.0f);
	}

	DirectionalLight( vec3 pPOS, vec3 pDirection, vec3 pColor) {
		direction = pDirection;
		POS = pPOS;
		color = pColor;
	}

	void updateShadowShader(GLuint& shaderProgram) {
		mat4 lightSpaceMatrix = getLightSpaceMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	}

	void updateSceneShader(GLuint& shaderProgram, string lightName) { updateSceneShader(shaderProgram, lightName, true); }

	void updateSceneShader(GLuint& shaderProgram, string lightName, bool enableShadows) {
		mat4 lightSpaceMatrix = getLightSpaceMatrix();

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, &(lightName + ".lightSpaceMatrix")[0]), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, &(lightName + ".lightDirection")[0]), 1, &direction[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, &(lightName + ".lightColor")[0]), 1, &color[0]);
		glUniform1i(glGetUniformLocation(shaderProgram, "enableShadows"), enableShadows);
	}

	mat4 getLightSpaceMatrix() {
		mat4 lightProjMatrix = ortho(-50.0f, 50.0f, -50.0f, 50.0f, nearPlane, farPlane);
		mat4 lightView = lookAt(POS, POS + direction, up);
		return lightProjMatrix * lightView;
	}
	
	vec3 direction;
	vec3 color;
	vec3 POS;
	float nearPlane = 5.0f;
	float farPlane = 150.0f;
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
};


#endif