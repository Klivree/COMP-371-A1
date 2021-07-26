#include "Model.hpp"


Model::Model(std::string pFilePath, int pNumOfShuffles, glm::vec3 pPOS, GLfloat pScale, GLenum pDrawMode) {
    initialFilePath = pFilePath;
    filePath = initialFilePath;
    numOfShuffles = pNumOfShuffles;

    initialPOS = pPOS;
    POS = initialPOS;

    initialScale = pScale;
    scale = initialScale;

    initialDrawMode = pDrawMode;
    drawMode = initialDrawMode;

    initialRotationVector = glm::vec3(0.0f);
    rotationVector = initialRotationVector;
}

void Model::resetModel() {
    POS = initialPOS;
    rotationVector = initialRotationVector;
    scale = initialScale;
    drawMode = initialDrawMode;
}

void Model::shuffle() {
    shuffleNumber = (shuffleNumber + 1) % numOfShuffles;

    if (shuffleNumber == 0)
        filePath = initialFilePath;
    else {
        std::string builtFilePath = std::string(initialFilePath);
        builtFilePath.erase(builtFilePath.length() - 4, builtFilePath.length());
        builtFilePath.append(" - Shuffle ");
        builtFilePath.append(std::to_string(shuffleNumber));
        builtFilePath.append(".csv");

        filePath = builtFilePath;
    }
        
}