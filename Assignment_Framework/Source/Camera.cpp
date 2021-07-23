#include "Camera.hpp"

Camera::Camera(int w, int h, glm::vec3 pos, float FOVdeg) {
	width = w;
	height = h;
	position = pos;
	initialPosition = position;
	orientation = initialOrientation;

	FOV = FOVdeg;
	initialFOV = FOV;
}

void Camera::createMatrices(float nearPlane, float farPlane, GLuint& shaderProgram) {
	/* creates and sends the view and projection matrices to the shader program
	* 
	*	FOVdeg - the FOV of the camera in degrees
	*	nearPlane - the near plane of the projection matrix
	*	farPlane - the far plane of the projection matrix
	*	shaderProgram - the loaded shader program of the OpenGL application
	*/

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);

	// creation of view and projection matrices
	viewMatrix = glm::lookAt(position, position + orientation, up);
	projectionMatrix = glm::perspective(glm::radians(FOV), (float)width / height, nearPlane, farPlane);

	// sending the view and projection matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
}

void Camera::processInputs(GLFWwindow* window, float dt) {
	/* Handles the inputs that will affect the camera position and view
	*/

	double mouseX, mouseY;

	// reset camera orientation and position
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_RELEASE)
		HomeLastStateReleased = true;
	else if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS && HomeLastStateReleased) {
		position = initialPosition;
		orientation = initialOrientation;
		FOV = initialFOV;

		HomeLastStateReleased = false;
	}


	// camera rotations
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		orientation = glm::rotateY(orientation, glm::radians(rotationSpeed * dt));
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		orientation = glm::rotateY(orientation, glm::radians(-rotationSpeed * dt));
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		orientation = glm::rotate(orientation, glm::radians(rotationSpeed * dt), glm::normalize(glm::cross(orientation, up)));
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		orientation = glm::rotate(orientation, glm::radians(-rotationSpeed * dt), glm::normalize(glm::cross(orientation, up)));

	///////MOUSE INPUTS///////
	// Tilt the image
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick) {
			glfwSetCursorPos(window, (float)width / 2, (float)height / 2);
			firstClick = false;
		}

		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = dt * sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = dt * sensitivity * (float)(mouseX - (height / 2)) / height;

		orientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up))); // right rotate
		orientation = glm::rotate(orientation, glm::radians(-rotY), up); // up rotate
	}
	// pan the image
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick) {
			glfwSetCursorPos(window, (float)width / 2, (float)height / 2); // put cursor in middle of window
			firstClick = false;
		}

		glfwGetCursorPos(window, &mouseX, &mouseY);

		//pan right
		position += speed * dt * (float)(mouseX - (width / 2)) * glm::normalize(glm::cross(orientation, up));
		//pan left
		position += speed * dt * (float)(mouseY - (height / 2)) * glm::cross(orientation, glm::normalize(glm::cross(orientation, up)));
	}
	// Zoom in and out
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick) {
			glfwSetCursorPos(window, (float)width / 2, (float)height / 2);
			firstClick = false;
		}

		glfwGetCursorPos(window, &mouseX, &mouseY);

		float newFOV = FOV + dt * (mouseY - (float)(height / 2));

		if (newFOV >= 5.0f && newFOV <= 150.0f) //check so that we don't flip the image
			FOV = newFOV;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		firstClick = true;
	}
}