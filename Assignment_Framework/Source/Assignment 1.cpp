#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "Camera.hpp"
#include "Model.hpp"
#include "PointLight.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace glm;

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _normal, vec2 _uv)
        : position(_position), uv(_uv), normal(_normal) {}

    vec3 position;
    vec3 normal;
    vec2 uv;
};

char* readFile(string filePath);

char* getVertexShaderSource(string vertexShaderFilePath) { return readFile(vertexShaderFilePath); }

char* getFragmentShaderSource(string fragmentShaderFilePath) { return readFile(fragmentShaderFilePath); }

GLuint getCubeModel();

GLuint getGridModel(glm::vec3 color);

GLuint compileAndLinkShaders(string vertexShaderFilePath, string fragmentShaderFilePath);

GLuint compileAndLinkShaders(string vertexShaderFilePath, string geometryShaderFilePath, string fragmentShaderFilePath);

void getShadowCubeMap(GLuint* frameBufferPtr, GLuint* texturePtr);

void renderGrid(GLuint shaderProgram);

void executeEvents(GLFWwindow* window, Camera& camera, float dt);

void renderLine(glm::vec3 pos, glm::vec3 size, glm::vec3 color, GLfloat scale, GLuint shaderProgram);

GLuint loadTexture(const char* filename);

void renderScene(GLuint shaderProgram);

// dimensions of the window in pixels
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

// dimensions of the shadow map
const int SHADOW_WIDTH = 1024;
const int SHADOW_HEIGHT = 1024;

const float initialFOV = 90.0f; // FOV of the initial player view in degrees
const GLfloat initialScale = 1.0f; // initial object scale
const GLenum defaultDrawMode = GL_TRIANGLES;

const glm::vec3 JackInitialPOS = glm::vec3(0.0f, 10.0f, 0.0f);
const glm::vec3 MelInitialPOS = glm::vec3(20.0f, 10.0f, 20.0f);
const glm::vec3 CedrikInitialPOS = glm::vec3(20.0f, 10.0f, -20.0f);
const glm::vec3 AlexInitialPOS = glm::vec3(-20.0f, 10.0f, 20.0f);
const glm::vec3 ThapanInitialPOS = glm::vec3(-20.0f, 10.0f, -20.0f);

string JacksShape = "../Assets/Shapes/Jack's Shape.csv";
string MelShape = "../Assets/Shapes/MelShape.csv";
string CedriksShape = "../Assets/Shapes/Cedrik's Shape.csv";
string AlexsShape = "../Assets/Shapes/Alex's Shape.csv";
string ThapansShape = "../Assets/Shapes/Thapan's Shape.csv";

vec3 wallPosOffset = vec3(0.0f, 0.0f, -5.0f);
string JacksWallPath = "../Assets/Shapes/Jack's Wall.csv";
string MelWallPath = "../Assets/Shapes/MelWall.csv";
string CedriksWallPath = "../Assets/Shapes/Cedrik's Wall.csv";
string AlexsWallPath = "../Assets/Shapes/Alex's Wall.csv";
string ThapansWallPath = "../Assets/Shapes/Thapan's Wall.csv";

//creation of model objects to remove switch statements in the executeEvents method
std::vector<Model *> objectModels;
std::vector<Model*> wallModels;

Model JacksModel = Model(JacksShape, JackInitialPOS, initialScale, GL_TRIANGLES);
Model MelModel = Model(MelShape, MelInitialPOS, initialScale, GL_TRIANGLES);
Model CedriksModel = Model(CedriksShape, CedrikInitialPOS, initialScale, GL_TRIANGLES);
Model AlexsModel = Model(AlexsShape, AlexInitialPOS, initialScale, GL_TRIANGLES);
Model ThapansModel = Model(ThapansShape, ThapanInitialPOS, initialScale, GL_TRIANGLES);

Model JacksWall = Model(JacksWallPath, JackInitialPOS + wallPosOffset, initialScale, GL_TRIANGLES);
Model MelWall = Model(MelWallPath, MelInitialPOS + wallPosOffset, initialScale, GL_TRIANGLES);
Model CedriksWall = Model(CedriksWallPath, CedrikInitialPOS + wallPosOffset, initialScale, GL_TRIANGLES);
Model AlexsWall = Model(AlexsWallPath, AlexInitialPOS + wallPosOffset, initialScale, GL_TRIANGLES);
Model ThapansWall = Model(ThapansWallPath, ThapanInitialPOS + wallPosOffset, initialScale, GL_TRIANGLES);

bool ModelSelection[] = { true, false, false, false, false };

bool enableShadows = true;
bool enableTextures = true;


int main(int argc, char* argv[]) {
    glfwInit(); //initialize GLFW

    //determine openGL version to initialize
#if defined(PLATFORM_OSX)	
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Comp371 - Assignment 1", NULL, NULL);
    if (window == NULL) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
  
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to create GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //get shader programs
    GLuint sceneShaderProgram = compileAndLinkShaders("../Assets/Shaders/vertexshader.glsl", "../Assets/Shaders/fragmentshader.glsl");
    GLuint shadowShaderProgram = compileAndLinkShaders("../Assets/Shaders/shadowvertexshader.glsl", "../Assets/Shaders/shadowgeometryshader.glsl", "../Assets/Shaders/shadowfragmentshader.glsl");

    // creation of the depth map framebuffer and texture [cube map is used since this is a point light and light is in 360 degrees around it]
    GLuint depthMapFBO, depthCubeMap;
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    for (int i = 0; i < 6; ++i) { //generating the 6 sides of the cube map
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    // telling the texture sampler the desired filtering methods, GL_NEAREST says to use the value of the nearest pixel
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // defining how to wrap the texture
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // get VAOs
    GLuint cubeModelVAO = getCubeModel();
    GLuint gridVAO = getGridModel(glm::vec3(1.0f, 1.0f, 0.0f));

    // generate the textures needed
    GLuint brickTexture = loadTexture("../Assets/Textures/brick.jpg");
    GLuint blankTexture = loadTexture("../Assets/Textures/blank.jpg");

    // initialize Materials
    vec3 goldVec(0.780392f, 0.568627f, 0.113725f);
    Material goldMaterial(goldVec, 0.8f);
    Material brickMaterial = Material(vec3(1.0f), 0.01f);

    objectModels.push_back(&JacksModel);
    objectModels.push_back(&MelModel);
    objectModels.push_back(&CedriksModel);
    objectModels.push_back(&AlexsModel);
    objectModels.push_back(&ThapansModel);   

    wallModels.push_back(&JacksWall);
    wallModels.push_back(&MelWall);
    wallModels.push_back(&CedriksWall);
    wallModels.push_back(&AlexsWall);
    wallModels.push_back(&ThapansWall);


    for (Model *object : objectModels) {
        object->linkVAO(cubeModelVAO, 36);
        object->linkTexture(blankTexture);
        object->setMaterial(goldMaterial);
    }

    for (Model *wall : wallModels) {
        wall->linkVAO(cubeModelVAO, 36);
        wall->linkTexture(brickTexture);
        wall->setMaterial(brickMaterial);
    }



    //generate camera
    Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 10.0f, 5.0f), initialFOV);

    //generate light
    PointLight light(vec3(0.0f, 30.0f, 0.0f), 200.0f, 1.0f, 0.007f, 0.002f, vec3(1.0f, 1.0f, 1.0f), SHADOW_HEIGHT);
    Material lightMaterial = Material(light.color, 0.01f);

    Model lightSource = Model("", light.POS, blankTexture);
    lightSource.linkVAO(cubeModelVAO, 36);
    lightSource.setMaterial(lightMaterial);
    lightSource.scale = 3.0f;

    // For frame time
    float lastFrameTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
 
    //Main loop
    while (!glfwWindowShouldClose(window)) {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;



        // render the depth map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); // change view to the size of the shadow texture
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); // bind the framebuffer
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shadowShaderProgram); // use proper shaders
        // update values in shadow shader
        light.updateShadowShader(shadowShaderProgram);
        renderScene(shadowShaderProgram); // render to make the texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind depth map FBO



        // render the scene as normal with the shadow mapping using the depth map
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // reset viewport tot hte size of the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(sceneShaderProgram);
        //update the values in the scene shader
        camera.createMatrices(0.01f, 100.0f, sceneShaderProgram);
        light.updateSceneShader(sceneShaderProgram, enableShadows);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

        renderScene(sceneShaderProgram);


        //render light not in two pass so it does not create shadows
        lightSource.render(sceneShaderProgram, true);

        glBindVertexArray(gridVAO);
        renderGrid(sceneShaderProgram);
        glBindVertexArray(0);

        //end frame
        glfwSwapBuffers(window); //swap the front buffer with back buffer
        glfwPollEvents(); //get inputs

        executeEvents(window, camera, dt);
    }

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void executeEvents(GLFWwindow* window, Camera& camera, float dt) {
    /* This function is used to execute all the inputs of the program
    * This program should only be called AFTER glfwPollEvents() was already called
    *
    *   window - current window of the program
    *   camera - camera class of the scene (needed so we can execute the camera inputs)
    *   dt - Frame time
    */

    //Note: these have to be static so that their state does not get reset on each function call
    static bool JLastReleased = true, ULastReleased = true,  SpaceLastReleased = true, BLastReleased = true, XLastReleased = true;
    static bool ALastReleased = true, DLastReleased = true, SLastReleased = true, WLastReleased = true, QLastReleased = true, ELastReleased = true;
    static bool ONELastReleased = true, TWOLastReleased = true, THREELastReleased = true, FOURLastReleased = true, FIVELastReleased = true;
    float scaleFactor = (float)8 / 7;
    float rotationFactor = 5.0f;
    float modelMovementSpeed = 2.0f;


    camera.processInputs(window, dt); // processes all camera inputs

    //change object
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
        ONELastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && ONELastReleased) {
        ModelSelection[0] = !ModelSelection[0];
        ONELastReleased = false;
    }


    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
        TWOLastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && TWOLastReleased) {
        ModelSelection[1] = !ModelSelection[1];
        TWOLastReleased = false;
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
        THREELastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && THREELastReleased) {
        ModelSelection[2] = !ModelSelection[2];
        THREELastReleased = false;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE)
        FOURLastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && FOURLastReleased) {
        ModelSelection[3] = !ModelSelection[3];
        FOURLastReleased = false;
    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE)
        FIVELastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && FIVELastReleased) {
        ModelSelection[4] = !ModelSelection[4];
        FIVELastReleased = false;
    }

    // reset the current model
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        SpaceLastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && SpaceLastReleased) {
        for (int i = 0; i < objectModels.size(); i++) {
            if (ModelSelection[i])
                objectModels[i]->resetModel();
        }
            
        SpaceLastReleased = false;
    }

    // toggle shadows off and on
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
        BLastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && BLastReleased) {
        enableShadows = abs(enableShadows - 1); // hacky way to toggle between true and false
        BLastReleased = false;
    }

    // toggle textures off and on
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
        XLastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && XLastReleased) {
        enableTextures = abs(enableTextures - 1); // hacky way to toggle between true and false
        XLastReleased = false;
    }

    // size up function
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE)
        ULastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && ULastReleased) {
        for (int i = 0; i < objectModels.size(); i++) {
            if (ModelSelection[i])
                objectModels[i]->scale *= scaleFactor;
        }
        ULastReleased = false;
    }
  

    // size down function
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE)
        JLastReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && JLastReleased) {
        for (int i = 0; i < objectModels.size(); i++) {
            if (ModelSelection[i])
                objectModels[i]->scale *= 1 / scaleFactor;
        }
        JLastReleased = false;
    }


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
        ALastReleased = true;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
        DLastReleased = true;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
        WLastReleased = true;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
        SLastReleased = true;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
        QLastReleased = true;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
        ELastReleased = true;


    // movement function
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->POS += glm::vec3(0.0f, modelMovementSpeed * dt, 0.0f);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->POS += glm::vec3(0.0f, -modelMovementSpeed * dt, 0.0f);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->POS += glm::vec3(-modelMovementSpeed * dt, 0.0f, 0.0f);
            }
        }
           
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->POS += glm::vec3(modelMovementSpeed * dt, 0.0f, 0.0f);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->POS += glm::vec3(0.0f, 0.0f, modelMovementSpeed * dt);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->POS += glm::vec3(0.0f, 0.0f, -modelMovementSpeed * dt);
            }
        }
    }
    else {
        // rotate counter-clockwise around positive y-axis
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && ALastReleased) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->rotationVector.y += rotationFactor;
            }
            ALastReleased = false;
        }

        // rotate clockwise around positive y-axis
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && DLastReleased) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->rotationVector.y -= rotationFactor;
            }
            DLastReleased = false;
        }

        // rotate counter-clockwise around positive z-axis
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && ELastReleased) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->rotationVector.z += rotationFactor;
            }
            ELastReleased = false;
        }

        // rotate clockwise around positive z-axis
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && QLastReleased) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->rotationVector.z -= rotationFactor;
            }
            QLastReleased = false;
        }

        // rotate counter-clockwise around positive x-axis
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && WLastReleased) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->rotationVector.x += rotationFactor;
            }
            WLastReleased = false;
        }

        // rotate clockwise around positive x-axis
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && SLastReleased) {
            for (int i = 0; i < objectModels.size(); i++) {
                if (ModelSelection[i])
                    objectModels[i]->rotationVector.x -= rotationFactor;
            }
            SLastReleased = false;
        }
    }


    // change the draw mode
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        for (int i = 0; i < objectModels.size(); i++) {
            if (ModelSelection[i])
                objectModels[i]->drawMode = GL_TRIANGLES;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        for (int i = 0; i < objectModels.size(); i++) {
            if (ModelSelection[i])
                objectModels[i]->drawMode = GL_LINES;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        for (int i = 0; i < objectModels.size(); i++) {
            if (ModelSelection[i])
                objectModels[i]->drawMode = GL_POINTS;
        }
    }

    // close the window on escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}

GLuint compileAndLinkShaders(string vertexShaderFilePath, string fragmentShaderFilePath){
    /* compile and link shader program
    * return shader program id
    */

    // create vertex shader
    const char* vertexShaderSource = readFile(vertexShaderFilePath);
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // create fragment shader
    const char* fragmentShaderSource = readFile(fragmentShaderFilePath);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    //create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //link the shader program
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    //delete shaders since they are already in the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint compileAndLinkShaders(string vertexShaderFilePath, string geometryShaderFilePath, string fragmentShaderFilePath) {
    /* compile and link shader program
* return shader program id
*/

// create vertex shader
    const char* vertexShaderSource = readFile(vertexShaderFilePath);
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // create geometry shader
    const char* geometryShaderSource = readFile(geometryShaderFilePath);
    GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
    glCompileShader(geometryShader);

    // check for shader compile errors
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // create fragment shader
    const char* fragmentShaderSource = readFile(fragmentShaderFilePath);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    //create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);
    //link the shader program
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    //delete shaders since they are already in the program
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void renderGrid(GLuint shaderProgram) {

    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

    // Draw Grid
    glm::mat4 pillarWorldMatrix = glm::mat4(1.0f);
    for (int i = 0; i < 100; ++i)
    {
        for (int j = 0; j < 100; ++j)
        {
            pillarWorldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f + i * 1.0f, -0.5f, -50.0f + j * 1.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
            glDrawArrays(GL_LINES, 0, 8);
        }
    }
}

void renderLine(glm::vec3 pos, glm::vec3 size, glm::vec3 color, GLfloat scale, GLuint shaderProgram) {
    /* Renders a line without the need to first load a VAO.
    *   pos - position to start drawing the line
    *   size - vector away from origin the other point is at
    *   color - color of the line
    *   scale - world scale to be used
    *   shaderProgram - shader program of the current application
    */

    glm::vec3 vertexArray[] = {
       pos, color,
       pos + (size * scale), color
    };

    GLuint VAO, VBO;
    //create the Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //create Vertex Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    //create position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    //create color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

    //draw the line
    glm::mat4 worldMatrix = glm::mat4(1.0); //ensure no transformation
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);

    //delete vertex arrays and buffer to prevent memory leak
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

GLuint getCubeModel() {
    /* Creates a cube model VAO.
    * This method should not be called in any loops as it will create a memory leak. Use before the main program loop
    *
    * returns the GLuint corresponding with the created VAO
    */

    // Cube model
    TexturedColoredVertex vertexArray[] = {  // position, normal, UV, color
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), //left 
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)), // far 
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)), // bottom
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)), // near
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)), // right
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))
    };

    GLuint VAO, VBO;
    //create the Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //create Vertex Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    //create position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)0);
    glEnableVertexAttribArray(0);

    //create normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    //create texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(2*sizeof(vec3)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

GLuint getGridModel(glm::vec3 color) {
    /* Creates a grid model VAO with the given color.
    * This method should not be called in any loops as it will create a memory leak. Use before the main program loop
    *
    *   color - color vector of the grid
    *
    * returns the GLuint corresponding with the created VAO
    */

    // Grid Model

    glm::vec3 vertexArray[] = {  // position,          color
        glm::vec3(0.5f, 0.5f, 0.5f), color, // C, top - yellow
        glm::vec3(0.5f, 0.5f,-0.5f), color,	// A
        glm::vec3(-0.5f, 0.5f,0.5f), color,	// D

        glm::vec3(-0.5f, 0.5f,-0.5f), color, // B

        glm::vec3(0.5f, 0.5f,-0.5f), color,	// A
        glm::vec3(-0.5f, 0.5f,-0.5f), color, // B

        glm::vec3(-0.5f, 0.5f,0.5f), color,	// D
        glm::vec3(0.5f, 0.5f, 0.5f), color, // C

    };

    GLuint VAO, VBO;
    //create the Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //create Vertex Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    //create position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    //create color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

char* readFile(string filePath) { //credit: https://badvertex.com/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c.html
    string content;
    ifstream fileStream(filePath, ios::in);

    if (!fileStream.is_open()) {
        cerr << "Could not read file " << filePath << ". File does not exist." << endl;
        return "";
    }

    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();

    char* chars = new char[content.length() + 1];
    strcpy(chars, content.c_str());

    return chars;
}

GLuint loadTexture(const char* filename) {
    // Step1 Create and bind textures
    GLuint textureId;
    glGenTextures(1, &textureId);
    
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

void getShadowCubeMap(GLuint* frameBufferPtr, GLuint* texturePtr) {

    glGenFramebuffers(1, frameBufferPtr);

    glGenTextures(1, texturePtr);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texturePtr);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    // telling the texture sampler the desired filtering methods, GL_NEAREST says to use the value of the nearest pixel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // defining how to wrap the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, *frameBufferPtr);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *texturePtr, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void renderScene(GLuint shaderProgram) {

    for (Model* object : objectModels)
        object->render(shaderProgram, enableTextures);

    for (Model* wall : wallModels)
        wall->render(shaderProgram, enableTextures);

    //render the origin lines
    //          position                    length                      color             scale
    renderLine(glm::vec3(0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, shaderProgram); // x direction
    renderLine(glm::vec3(0.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, shaderProgram); // y direction
    renderLine(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, shaderProgram); // z direction

}

