#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <irrKlang.h> // for sound
#include "Camera.hpp"
#include "Model.hpp"
#include "PointLight.hpp"
#include "OBJLoader.hpp"
#include "WallBuilder.hpp"
#include "TextRenderer.hpp"
#include "Grouping.hpp"
#include "SpotLight.hpp"
#include "DirectionalLight.hpp"

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

void printVec3(vec3 vector3) { cout << vector3.x << ", " << vector3.y << ", " << vector3.z << endl; }

char* readFile(string filePath);

GLuint getCubeModel();

GLuint compileAndLinkShaders(string vertexShaderFilePath, string fragmentShaderFilePath);

GLuint compileAndLinkShaders(string vertexShaderFilePath, string geometryShaderFilePath, string fragmentShaderFilePath);

void getShadowCubeMap(GLuint* depthMapFBO, GLuint* depthCubeMa);

void renderGrid(GLuint shaderProgram);

void executeEvents(GLFWwindow* window, Camera& camera, float dt);

void initializeModels();

GLuint loadTexture(const char* filename);

void renderScene(GLuint shaderProgram);

void shapePassedWall();

vec3 generateStartingAngle();

void window_size_callback(GLFWwindow* window, int width, int height);

GLuint setupModelVBO(string path, int& vertexCount);

// dimensions of the window in pixels
int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 768;

// dimensions of the shadow map
const int SHADOW_WIDTH = 1024;
const int SHADOW_HEIGHT = 1024;

const float initialFOV = 90.0f; // FOV of the initial player view in degrees
const GLfloat initialScale = 1.0f; // initial object scale

vec3 wallPosOffset = vec3(0.0f, 0.0f, -5.0f);
vec3 objectStartingPoint = vec3(0.0f, 0.0f, -10.0f);

//creation of model objects to remove switch statements in the executeEvents method
vector<Model*> groundModels;

Model shapeModel = Model("../Assets/Shapes/SHC/SHC-LVL2.csv", objectStartingPoint, initialScale, GL_TRIANGLES);

Model wallModel = Model(buildWall(shapeModel.getFilePath()), vec3(0.0f), initialScale, GL_TRIANGLES);

Model axisModelX = Model("../Assets/Shapes/Axis/XLine.csv", vec3(2.5f, 0.0f, 0.0f), 1.0f, GL_TRIANGLES);
Model axisModelY = Model("../Assets/Shapes/Axis/YLine.csv", vec3(0.0f, 2.5f, 0.0f), 1.0f, GL_TRIANGLES);
Model axisModelZ = Model("../Assets/Shapes/Axis/ZLine.csv", vec3(0.0f, 0.0f, 2.5f), 1.0f, GL_TRIANGLES);

Model axisModelXSHAPE = Model("../Assets/Shapes/Axis/XLine.csv", vec3(2.5f, 0.0f, 0.0f), 1.0f, GL_TRIANGLES);
Model axisModelYSHAPE = Model("../Assets/Shapes/Axis/YLine.csv", vec3(0.0f, 2.5f, 0.0f), 1.0f, GL_TRIANGLES);
Model axisModelZSHAPE = Model("../Assets/Shapes/Axis/ZLine.csv", vec3(0.0f, 0.0f, 2.5f), 1.0f, GL_TRIANGLES);

Model GroundFloor = Model("../Assets/Shapes/Ground.csv", glm::vec3(0.0f, -25.0f, 0.0f), initialScale, GL_TRIANGLES);

Model pepeModel1 = Model("../Assets/Shapes/Basic.csv", vec3(-5.0f, 0.0f, -2.5f), 0.10f, GL_TRIANGLES);
Model pepeModel2 = Model("../Assets/Shapes/Basic.csv", vec3(7.5f, 0.0f, -2.5f), 0.10f, GL_TRIANGLES);

Grouping axises, axisesShape;

irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();

bool ModelSelection[] = { true, false, false, false, false };

bool enableShadows = true;
bool enableTextures = true;

// placed in case we want to cycle through light colors in SUPERHYPERCUBE game
vec3 lightColors[] = {
    vec3((float)128 / 255, (float)0 / 255, (float)0 / 255), // maroon
    vec3((float)170 / 255, (float)110 / 255, (float)40 / 255), // brown
    vec3((float)128 / 255, (float)128 / 255, (float)0 / 255), // olive
    vec3((float)0 / 255, (float)128 / 255, (float)128 / 255), // teal
    vec3((float)0 / 255, (float)0 / 255, (float)128 / 255), // navy
    vec3((float)230 / 255, (float)25 / 255, (float)75 / 255), // red
    vec3((float)245 / 255, (float)130 / 255, (float)48 / 255), // orange
    vec3((float)255 / 255, (float)225 / 255, (float)25 / 255), // yellow
    vec3((float)60 / 255, (float)180 / 255, (float)75 / 255), // green
    vec3((float)70 / 255, (float)240 / 255, (float)240 / 255), // cyan
    vec3((float)0 / 255, (float)130 / 255, (float)200 / 255), // blue
    vec3((float)145 / 255, (float)30 / 255, (float)180 / 255), // purple
    vec3((float)240 / 255, (float)50 / 255, (float)230 / 255), // magenta
    vec3((float)255 / 255, (float)255 / 255, (float)255 / 255), // white
};
int lightColorIndex = 0;

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
    GLuint textShaderProgram = compileAndLinkShaders("../Assets/Shaders/textvertexshader.glsl", "../Assets/Shaders/textfragmentshader.glsl");

    // creation of the depth map framebuffer and texture [cube map is used since this is a point light and light is in 360 degrees around it]
    GLuint depthMapFBO, depthCubeMap;
    getShadowCubeMap(&depthMapFBO, &depthCubeMap);

    initializeModels();

    // generate camera
    Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 10.0f, 5.0f), initialFOV);

    // generate light
    PointLight light(vec3(0.0f, 10.0f, 0.0f), 200.0f, 1.0f, 0.007f, 0.002f, vec3(1.0f), SHADOW_HEIGHT);
    // this is made outside of initilize models call so that we can get the lights position
    Model lightSource = Model("", light.POS, loadTexture("../Assets/Textures/blank.jpg"));
    lightSource.linkVAO(getCubeModel(), 36);
    lightSource.setMaterial(Material(light.color, 0.01f));
    lightSource.scale = 3.0f;

    Spotlight spotLight1 = Spotlight(vec3(0.0f, 15.0f, 0.0f), vec3(0.0f, -1.0f, -0.5f), vec3(0.5f, 1.0f, 0.25f), radians(20.0f), radians(30.0f));

    // For frame time
    float lastFrameTime = glfwGetTime();

    srand(time(NULL));

    //make the textures point to the right position
    glUseProgram(sceneShaderProgram);
    glUniform1i(glGetUniformLocation(sceneShaderProgram, "modelTexture"), 0);
    glUniform1i(glGetUniformLocation(sceneShaderProgram, "shadowMap"), 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    axises.setPOS(shapeModel.POS + vec3(2.0f, 0.0f, 2.0f));
    axisesShape.setPOS(shapeModel.POS + vec3(0.0f, -2.0f, 0.0f));

    soundEngine->play2D("../Assets/Sounds/Breakout.mp3", true); // music courtesy of https://learnopengl.com

    //Main loop
    while (!glfwWindowShouldClose(window)) {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

		// window size callback called when window size changes
		glfwSetWindowSizeCallback(window, window_size_callback);

        // bind camera to object
        camera.position = shapeModel.POS + vec3(0.0f, 2.0f, -4.0f);
        camera.orientation = normalize(shapeModel.POS - camera.position);

        axisesShape.setRotationVector(shapeModel.rotationVector); // for debugging 

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
        camera.createMatrices(0.01f, 200.0f, sceneShaderProgram, WINDOW_WIDTH, WINDOW_HEIGHT);
        light.updateSceneShader(sceneShaderProgram, "pointlight1", enableShadows);
        spotLight1.updateSceneShader(sceneShaderProgram, "spotlight1");
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
        renderScene(sceneShaderProgram);

        //render light not in two pass so it does not create shadows
        glUniform1i(glGetUniformLocation(sceneShaderProgram, "enableShadows"), false);
        glUniform1i(glGetUniformLocation(sceneShaderProgram, "fullLight"), true);
        lightSource.render(sceneShaderProgram, true);
        glUniform1i(glGetUniformLocation(sceneShaderProgram, "fullLight"), false);
        glUniform1i(glGetUniformLocation(sceneShaderProgram, "enableShadows"), enableShadows);

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
    static bool BLastReleased = true, XLastReleased = true;
    float scaleFactor = (float)8 / 7;
    float rotationFactor = 5.0f;
    static float modelMovementSpeed = 1.0f;
    float slowMovementSpeed = 1.0f;
    float fastMovementSpeed = 5.0f;

    camera.processInputs(window, dt); // processes all camera inputs

    ////////////////////////////// 90 DEGREE ROTATIONS FOR SUPERHYPERCUBE GAME - DO NOT REMOVE - WILL BE UNCOMMENTED FOR FINAL PROJECT ////////////////////////////
    //variables for game
    static float currentDeg = 0.0f;
    float goalDeg = 90.0f;
    float rateOfRotation = 270.0f;
    static bool rotating = false;
    static vec3 rotationAxis = vec3(1.0f, 0.0f, 0.0f);
    static vec3 initialRotationAxis = shapeModel.rotationVector;


    // make shape go towards wall
    shapeModel.POS += vec3(0.0f, 0.0f, modelMovementSpeed * dt);

    if (shapeModel.POS.z > 1.0f) {
        modelMovementSpeed = slowMovementSpeed;
        shapePassedWall(); // called when we need a reset and a new model
    }

    if (rotating) {
        if (currentDeg < goalDeg) { // continue rotation loop
            currentDeg += rateOfRotation * dt;
            shapeModel.rotationVector += rateOfRotation * dt * rotationAxis; // rotate model
        }
        else { // end rotation loop
            // make sure that the model rotates exactly the amount of degrees that we want (shouldn't look too jank since it should be fairly close to the desired degree)
            shapeModel.rotationVector = initialRotationAxis + (goalDeg * rotationAxis);

            // reset flags for next rotation
            currentDeg = 0;
            rotating = false;
        }
    }
    else{
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            rotating = true;

            rotationAxis = vec3(1.0f, 0.0f, 0.0f); // direction of rotation
        }
 
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            rotating = true;

            rotationAxis = vec3(-1.0f, 0.0f, 0.0f); // direction of rotation
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            rotating = true;

            rotationAxis = vec3(0.0f, -1.0f, 0.0f); // direction of rotation
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            rotating = true;

            rotationAxis = vec3(0.0f, 1.0f, 0.0f); // direction of rotation
        }
        else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            rotating = true;
            rotationAxis = vec3(0.0f, 0.0f, 1.0f); // direction of rotation
        }
        else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            rotating = true;

            rotationAxis = vec3(0.0f, 0.0f, -1.0f); // direction of rotation
        }

        if (rotating) 
            initialRotationAxis = shapeModel.rotationVector; // to ensure we do a full 90 degree rotatio
    } 

    // speed up model
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        modelMovementSpeed = slowMovementSpeed;
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
        modelMovementSpeed = fastMovementSpeed;
    

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

    // close the window on escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}

void renderScene(GLuint shaderProgram) {

    pepeModel1.render(shaderProgram);
    pepeModel2.render(shaderProgram);

    wallModel.render(shaderProgram, enableTextures);

    shapeModel.render(shaderProgram, enableTextures);


    for (Model* ground : groundModels)
        ground->render(shaderProgram, enableTextures);

    axises.render(shaderProgram, enableTextures);
    axisesShape.render(shaderProgram, enableTextures);
}

void initializeModels() {
    // get VAOs
    GLuint cubeModelVAO = getCubeModel();

    // generate the textures needed
    GLuint brickTexture = loadTexture("../Assets/Textures/brick.jpg");
    GLuint blankTexture = loadTexture("../Assets/Textures/blank.jpg");
    GLuint tileTexture = loadTexture("../Assets/Textures/tile.png");
    GLuint goldTexture = loadTexture("../Assets/Textures/metal.jpg");

    // initialize Materials
    vec3 goldVec(0.780392f * 1.5f, 0.568627f * 1.5f, 0.113725f * 1.5f);
    Material goldMaterial(goldVec, 1.0f);
    Material brickMaterial = Material(vec3(1.0f), 0.01f);
    Material tileMaterial = Material(vec3(1.0f), 0.2f);
    

    groundModels.push_back(&GroundFloor);

    axisModelX.setMaterial(Material(vec3(0.0f, 0.0f, 1.0f), 0.2f));
    axisModelX.linkVAO(cubeModelVAO, 36);
    axisModelX.linkTexture(blankTexture);

    axisModelY.setMaterial(Material(vec3(0.0f, 1.0f, 0.0f), 0.2f));
    axisModelY.linkVAO(cubeModelVAO, 36);
    axisModelY.linkTexture(blankTexture);

    axisModelZ.setMaterial(Material(vec3(1.0f, 0.0f, 0.0f), 0.2f));
    axisModelZ.linkVAO(cubeModelVAO, 36);
    axisModelZ.linkTexture(blankTexture);

    axises.addToGrouping(axisModelX);
    axises.addToGrouping(axisModelY);
    axises.addToGrouping(axisModelZ);


    axisModelXSHAPE.setMaterial(Material(vec3(0.0f, 0.0f, 1.0f), 0.2f));
    axisModelXSHAPE.linkVAO(cubeModelVAO, 36);
    axisModelXSHAPE.linkTexture(blankTexture);

    axisModelYSHAPE.setMaterial(Material(vec3(0.0f, 1.0f, 0.0f), 0.2f));
    axisModelYSHAPE.linkVAO(cubeModelVAO, 36);
    axisModelYSHAPE.linkTexture(blankTexture);

    axisModelZSHAPE.setMaterial(Material(vec3(1.0f, 0.0f, 0.0f), 0.2f));
    axisModelZSHAPE.linkVAO(cubeModelVAO, 36);
    axisModelZSHAPE.linkTexture(blankTexture);

    axisesShape.addToGrouping(axisModelXSHAPE);
    axisesShape.addToGrouping(axisModelYSHAPE);
    axisesShape.addToGrouping(axisModelZSHAPE);

    shapeModel.setMaterial(goldMaterial);
    shapeModel.linkVAO(cubeModelVAO, 36);
    shapeModel.linkTexture(goldTexture);

    wallModel.setMaterial(brickMaterial);
    wallModel.linkVAO(cubeModelVAO, 36);
    wallModel.linkTexture(brickTexture);

    // for once we're ready to unleash the beast
    int pepeVertices;
    Material pepeMaterial = Material(vec3((float)43 / 255, (float)106 / 255, (float)64 / 255), 0.2f);
    GLuint pepeVAO = setupModelVBO("../Assets/Models/Pepe.obj", pepeVertices);
    pepeModel1.linkVAO(pepeVAO, pepeVertices);
    pepeModel1.setMaterial(pepeMaterial);
    pepeModel1.linkTexture(blankTexture);
    pepeModel1.rotationVector += vec3(0.0f, 90.0f, 0.0f);
    
    pepeModel2.linkVAO(pepeVAO, pepeVertices);
    pepeModel2.setMaterial(pepeMaterial);
    pepeModel2.linkTexture(blankTexture);
    pepeModel2.rotationVector += vec3(0.0f, 90.0f, 0.0f);


    for (Model* ground : groundModels) {
        ground->linkVAO(cubeModelVAO, 36);
        ground->texWrapX = 8.0f;
        ground->texWrapY = 8.0f;
        ground->linkTexture(tileTexture);
        ground->setMaterial(tileMaterial);
    }
}

void shapePassedWall() {
    shapeModel.resetModel();
    shapeModel.rotationVector = generateStartingAngle();
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

GLuint getCubeModel() {
    /* Creates a cube model VAO.
    * This method should not be called in any loops as it will create a memory leak. Use before the main program loop
    *
    * returns the GLuint corresponding with the created VAO
    */

    // Cube model
    TexturedColoredVertex vertexArray[] = {  // position, normal, UV, color
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)), //left 
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)), // far 
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)), // bottom
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)), // near
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), // right
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
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

vec3 generateStartingAngle() {
    float possibleAngles[] = { 0.0f, 90.0f, 180.0f, 270.0f };
    int arraySize = 4;
    return vec3(possibleAngles[rand() % arraySize], possibleAngles[rand() % arraySize], possibleAngles[rand() % arraySize]);
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

void getShadowCubeMap(GLuint* depthMapFBO, GLuint* depthCubeMap) {
    glGenFramebuffers(1, depthMapFBO);

    glGenTextures(1, depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *depthCubeMap);
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

    glBindFramebuffer(GL_FRAMEBUFFER, *depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}

GLuint setupModelVBO(string path, int& vertexCount) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> UVs;

    //read the vertex data from the model's OBJ file
    loadOBJ(path.c_str(), vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);
    

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
    vertexCount = vertices.size();
    return VAO;
}
