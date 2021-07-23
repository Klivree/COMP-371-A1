#include "Camera.hpp"

Camera::Camera(int w, int h, glm::vec3 pos) {
	width = w;
	height = h;
	position = pos;
	initialPosition = position;
	orientation = initialOrientation;
}

void Camera::createMatrices(float FOVdeg, float nearPlane, float farPlane, GLuint& shaderProgram) {
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
	projectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

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

		HomeLastStateReleased = false;
	}


	// camera rotations
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		orientation = glm::rotateY(orientation, glm::radians(rotationSpeed * dt));
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		orientation = glm::rotateY(orientation, glm::radians(-rotationSpeed * dt));
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		orientation = glm::rotateX(orientation, glm::radians(rotationSpeed * dt));
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		orientation = glm::rotateX(orientation, glm::radians(-rotationSpeed * dt));

	// camera positions
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) != GLFW_PRESS) {
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
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstRightClick) {
			glfwSetCursorPos(window, (float)width / 2, (float)height / 2); // put cursor in middle of window
			firstRightClick = false;
		}

		glfwGetCursorPos(window, &mouseX, &mouseY);

		position += glm::cross(orientation,glm::vec3(speed * (float)(mouseY - (height / 2)) * dt, speed * (float)(mouseX - (width / 2)) * dt, 0.0f));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && firstMiddleClick) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		firstRightClick = true;
	}


	

	// mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstMiddleClick) {
			glfwSetCursorPos(window, (float) width / 2, (float) height / 2);
			firstMiddleClick = false;
		}

		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = dt * sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = dt * sensitivity * (float)(mouseX - (height / 2)) / height;

		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));
		
		if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -up) <= glm::radians(5.0f))))
			orientation = newOrientation;
		
	
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && firstRightClick) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		firstMiddleClick = true;
	}
}