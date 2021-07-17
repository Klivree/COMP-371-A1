#include "Camera.hpp"

Camera::Camera(int w, int h, glm::vec3 pos) {
	width = w;
	height = h;
	position = pos;
}

void Camera::matrix(float FOVdeg, float nearPlane, float farPlane, GLuint& shaderProgram) {
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);

	viewMatrix = glm::lookAt(position, position + orientation, up);
	projectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
}

void Camera::inputs(GLFWwindow *window, float dt) {
	// camera positions
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += dt * speed * orientation;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position += dt * speed * -orientation;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += dt * speed * glm::normalize(glm::cross(orientation, up));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position += dt * speed * -glm::normalize(glm::cross(orientation, up));
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += dt * speed * up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		position += dt * speed * -up;

	// speed ajustments
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed = 5.0f;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		speed = 1.0f;

	// mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick) {
			glfwSetCursorPos(window, (float) width / 2, (float) height / 2);
			firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = dt * sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = dt * sensitivity * (float)(mouseX - (height / 2)) / height;

		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));
		
		if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -up) <= glm::radians(5.0f))))
			orientation = newOrientation;
		
	
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		//glfwSetCursorPos(window, (float) width/2, (float) height/2);

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		firstClick = true;
	}
}