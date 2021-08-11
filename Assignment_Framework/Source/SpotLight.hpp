#ifndef SPOT_LIGHT_CLASS
#define SPOT_LIGHT_CLASS

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

using namespace std;
using namespace glm;

class Spotlight {
public:
	Spotlight(vec3 pPOS, vec3 pDir, float pCutoffInner, float pCutoffOuter) {
		POS = pPOS;
		direction = pDir;
		color = vec3(1.0f);
		cutOffInner = pCutoffInner;
		cutOffOuter = pCutoffOuter;
	}

	Spotlight(vec3 pPOS, vec3 pDir, vec3 pColor, float pCutoffInner, float pCutoffOuter) {
		POS = pPOS;
		direction = pDir;
		color = pColor;
		cutOffInner = pCutoffInner;
		cutOffOuter = pCutoffOuter;
	}

	void updateSceneShader(GLuint& shaderProgram, string lightName) {

		glUniform3fv(glGetUniformLocation(shaderProgram, &(lightName + ".POS")[0]), 1, &POS[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, &(lightName + ".lightDirection")[0]), 1, &direction[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, &(lightName + ".lightColor")[0]), 1, &color[0]);
		glUniform1f(glGetUniformLocation(shaderProgram, &(lightName + ".cutOffInner")[0]), cutOffInner);
		glUniform1f(glGetUniformLocation(shaderProgram, &(lightName + ".cutOffOuter")[0]), cutOffOuter);

	}

	vec3 direction;
	vec3 color;
	vec3 POS;
	float cutOffInner;
	float cutOffOuter;
};


#endif