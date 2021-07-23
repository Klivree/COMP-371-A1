#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <string>
#include <fstream>
#include <iostream>

class Model {
public:
	Model(std::string pFilePath, GLuint pVAO, glm::vec3 pPosition, GLenum pDrawMode);

	void render(GLuint shaderProgram);

	void resetModel();

private:
	glm::vec3 position;
	glm::vec3 rotationVector;
	GLfloat scale;
	GLenum drawMode;

	GLfloat initialScale = 1.0f;
	glm::vec3 initialRotationVector = glm::vec3(0.0f);
	GLenum defaultDrawMode;
	glm::vec3 initialPOS;
	std::string filepath;
	GLuint VAO;
};







#endif