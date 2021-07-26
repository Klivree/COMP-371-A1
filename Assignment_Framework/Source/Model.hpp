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
#include <string>

class Model {
public:
    Model(std::string pFilePath, int pNumOfShuffles, glm::vec3 pPOS, GLfloat pScale, GLenum pDrawMode);

    void resetModel();

    void shuffle();

    glm::vec3 POS;
    glm::vec3 rotationVector;
    std::string filePath;
    GLfloat scale;
    GLenum drawMode;

private:
    glm::vec3 initialPOS;
    glm::vec3 initialRotationVector;
    GLfloat initialScale;
    GLenum initialDrawMode;
    std::string initialFilePath;
    int numOfShuffles;
    int shuffleNumber = 0;
};

#endif