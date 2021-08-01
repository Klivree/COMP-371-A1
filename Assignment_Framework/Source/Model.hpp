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
#include <vector>

struct Material { // sample material values can be obtained from : http://devernay.free.fr/cours/opengl/materials.html
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    Material() { // default white plastice material
        ambient = glm::vec3(1.0f, 1.0f, 1.0f);
        diffuse = glm::vec3(0.55f, 0.55f, 0.55f);
        specular = glm::vec3(0.70f, 0.70f, 0.70f);
        shininess = 0.1f;
    }

    Material(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _shininess) {
        ambient = _ambient;
        diffuse = _diffuse;
        specular = _specular;
        shininess = _shininess;
    }
};

struct cubeInfo {
    GLfloat posX;
    GLfloat posY;
    GLfloat posZ;
    GLfloat scaleX;
    GLfloat scaleY;
    GLfloat scaleZ;

    cubeInfo(GLfloat _posX, GLfloat _posY, GLfloat _posZ, GLfloat _scaleX, GLfloat _scaleY, GLfloat _scaleZ) {
        posX = _posX;
        posY = _posY;
        posZ = _posZ;
        scaleX = _scaleX;
        scaleY = _scaleY;
        scaleZ = _scaleZ;
    }
};

class Model {
public:
    Model(std::string pFilePath, glm::vec3 pPOS, GLfloat pScale, GLenum pDrawMode);

    Model(std::string pFilePath, glm::vec3 pPOS, GLfloat pScale, GLuint pTexture, GLenum pDrawMode);

    Model(std::string pFilePath, glm::vec3 pPOS, GLuint pTexture);

    Model(std::string pFilePath, glm::vec3 pPOS, GLuint pVAO, GLuint pTexture, Material pMaterial);

    void resetModel();

    void render(GLuint shaderProgram);

    void render(GLuint shaderProgram, bool enableTextures);

    void linkVAO(GLuint pVAO, int pActiveVertices);

    void linkTexture(GLuint pTexture);

    void setMaterial(Material pMaterial);

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

    std::vector<cubeInfo> information;

    GLuint VAO;
    int activeVertices;

    GLuint texture;

    Material material;

    void initializeModel();
};

#endif