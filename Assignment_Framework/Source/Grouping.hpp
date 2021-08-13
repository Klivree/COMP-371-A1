#ifndef GROUPING_HEADER
#define GROUPING_HEADER

#include <vector>
#include <glm/glm.hpp>
#include <glew-2.1.0/include/GL/glew.h>
#include "Model.hpp"

class Grouping {
public:
	Grouping() {
	};

	void move(glm::vec3 movementVector) {
		POS += movementVector;
	}

	void rotate(glm::vec3 pRotationVector) {
		rotationVector += pRotationVector;
	}

	void render(GLuint shaderProgram, bool enableTextures) {
		mat4 baseMatrix = mat4(1.0f);
		baseMatrix = glm::translate(baseMatrix, POS);
		baseMatrix = glm::scale(baseMatrix, glm::vec3(scale));
		baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationVector.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate around x axis
		baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationVector.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate around y axis
		baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationVector.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate around z axis

		for (Model* model : groupedModels) {
			model->render(shaderProgram, enableTextures, baseMatrix);
		}
	}

	void addToGrouping(Model& pModel) {
		if (groupedModels.size() == 0)
			POS = pModel.POS;
		groupedModels.push_back(&pModel);
	}

	vec3 getPOS() { return POS; }

	void setPOS(vec3 pPOS) { POS = pPOS; }

	vec3 getRotationVector() { return rotationVector; }

	void setRotationVector(vec3 pRotationVector) { rotationVector = pRotationVector; }

	void setScale(float pScale) { scale = pScale; }

	float getScale() { return scale; }

private:
	std::vector<Model*> groupedModels;
	vec3 POS;
	vec3 rotationVector = vec3(0.0f);
	float scale = 1.0f;
};



#endif