#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace glm;
using namespace std;

struct Material { // sample material values can be obtained from : http://devernay.free.fr/cours/opengl/materials.html
    vec3 color;
    float shininess;

    Material() { // default white plastice material
        color = vec3(1.0f, 1.0f, 1.0f);
        shininess = 0.1f;
    }

    Material(vec3 _color, float _shininess) {
        color = _color;
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
    Model(string pFilePath, vec3 pPOS, GLfloat pScale, GLenum pDrawMode);

    Model(string pFilePath, vec3 pPOS, GLuint pTexture);

    void resetModel();

    void render(GLuint shaderProgram);

    void render(GLuint shaderProgram, bool enableTextures);

    void render(GLuint shaderProgram, bool enableTextures, mat4 baseMatrix);

    void linkVAO(GLuint pVAO, int pActiveVertices);

    void linkTexture(GLuint pTexture);

    void setMaterial(Material pMaterial);

    void updateFilePath(std::string pFilePath);

    string getFilePath();

    vec3 POS;
    quat rotationQuat;
    GLfloat scale;
    GLenum drawMode;

    // amount of times to repeat the texture
    float texWrapX = 1.0f;
    float texWrapY = 1.0f;



private:
    vec3 initialPOS;
    quat initialQuat;
    GLfloat initialScale;
    GLenum initialDrawMode;
    string filePath;

    vector<cubeInfo> information;

    GLuint VAO;
    int activeVertices;

    GLuint texture;

    Material material;

    void initializeModel();
};

#endif