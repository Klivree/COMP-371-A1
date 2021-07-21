#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

class Camera {
public:
	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	int width, height;

	bool firstClick = true;

	float speed = 1.0f;
	float sensitivity = 100.0f;

	Camera(int width, int height, glm::vec3 pos);

	void createMatrices(float FOVdeg, float nearPlane, float farPlane, GLuint& shaderProgram);

	void processInputs(GLFWwindow* window, float dt);	

};

#endif