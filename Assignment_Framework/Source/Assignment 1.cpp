#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
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
#include <time.h>

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

quat generateStartingAngle();

void window_size_callback(GLFWwindow* window, int width, int height);

GLuint setupModelVBO(string path, int& vertexCount);

void endGame();


//////////////////////////////////////////////// WINDOW CONSTANTS ////////////////////////////////////////////////
// dimensions of the window in pixels
int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 768;

// dimensions of the shadow map
const int SHADOW_WIDTH = 1024;
const int SHADOW_HEIGHT = 1024;

//////////////////////////////////////////////// OBJECT POSTION CONSTANTS ////////////////////////////////////////////////
vec3 wallPosOffset = vec3(0.0f, 0.0f, -10.0f);
vec3 objectStartingPoint = vec3(0.0f, 0.0f, -15.0f);
vec3 scoreTextPosition = vec3(-0.95f, 0.95f, 0.0f);
vec3 timeTextPosition = scoreTextPosition + vec3(1.55f, 0.0f, 0.0f);

//////////////////////////////////////////////// PATHS OF ALL THE OBJECTS ////////////////////////////////////////////////
vector<string> shapePaths = {
	"../Assets/Shapes/SHC/SHC-LVL1.csv",
	"../Assets/Shapes/SHC/SHC-LVL2.csv",
	"../Assets/Shapes/SHC/SHC-LVL3.csv",
	"../Assets/Shapes/SHC/SHC-LVL4.csv",
	"../Assets/Shapes/SHC/SHC-LVL5.csv",
    "../Assets/Shapes/SHC/SHC-LVL6.csv",
    "../Assets/Shapes/SHC/SHC-LVL7.csv",
    "../Assets/Shapes/SHC/SHC-LVL8.csv",
    "../Assets/Shapes/SHC/SHC-LVL9.csv",
    "../Assets/Shapes/SHC/SHC-LVL10.csv",
};
int currentShape = 0;

//////////////////////////////////////////////// GENERATE MODELS ////////////////////////////////////////////////
Model shapeModel = Model(shapePaths[currentShape], objectStartingPoint, 1.0f, GL_TRIANGLES);

Model wallModel = Model(buildWall(shapeModel.getFilePath()), vec3(0.0f), 1.0f, GL_TRIANGLES);

Model GroundFloor = Model("../Assets/Shapes/Ground.csv", glm::vec3(0.0f, -25.0f, 0.0f), 1.0f, GL_TRIANGLES);

Model pepeModel1 = Model("../Assets/Shapes/Basic.csv", vec3(20.0f, -22.0f, 15.0f), 0.10f, GL_TRIANGLES);
Model pepeModel2 = Model("../Assets/Shapes/Basic.csv", vec3(-20.0f, -22.0f, 15.0f), 0.10f, GL_TRIANGLES);
Model pepeModel3 = Model("../Assets/Shapes/Basic.csv", vec3(20.0f, -22.0f, 5.0f), 0.10f, GL_TRIANGLES);
Model pepeModel4 = Model("../Assets/Shapes/Basic.csv", vec3(-20.0f, -22.0f, 5.0f), 0.10f, GL_TRIANGLES);
Model pepeModel5 = Model("../Assets/Shapes/Basic.csv", vec3(20.0f, -22.0f, -5.0f), 0.10f, GL_TRIANGLES);
Model pepeModel6 = Model("../Assets/Shapes/Basic.csv", vec3(-20.0f, -22.0f, -5.0f), 0.10f, GL_TRIANGLES);

// vector for pepe models since they are initialized and modeled in a similar fashion
vector<Model*> pepeModels = { &pepeModel1,&pepeModel2,&pepeModel3,&pepeModel4,&pepeModel5,&pepeModel6 };


//////////////////////////////////////////////// LIGHTS ////////////////////////////////////////////////
vec3 pepeLightColor = vec3((float)255 / 255, (float)132 / 255, (float)0 / 255);
PointLight pepeLight1 = PointLight(pepeModels[0]->POS + vec3(0.0f, 10.0f, 0.0f), 150.0f, 1.0, 0.045, 0.0075, pepeLightColor, SHADOW_HEIGHT);
PointLight pepeLight2 = PointLight(pepeModels[1]->POS + vec3(0.0f, 10.0f, 0.0f), 150.0f, 1.0, 0.045, 0.0075, pepeLightColor, SHADOW_HEIGHT);
PointLight pepeLight3 = PointLight(pepeModels[2]->POS + vec3(0.0f, 10.0f, 0.0f), 150.0f, 1.0, 0.045, 0.0075, pepeLightColor, SHADOW_HEIGHT);
PointLight pepeLight4 = PointLight(pepeModels[3]->POS + vec3(0.0f, 10.0f, 0.0f), 150.0f, 1.0, 0.045, 0.0075, pepeLightColor, SHADOW_HEIGHT);
PointLight pepeLight5 = PointLight(pepeModels[4]->POS + vec3(0.0f, 10.0f, 0.0f), 150.0f, 1.0, 0.045, 0.0075, pepeLightColor, SHADOW_HEIGHT);
PointLight pepeLight6 = PointLight(pepeModels[5]->POS + vec3(0.0f, 10.0f, 0.0f), 150.0f, 1.0, 0.045, 0.0075, pepeLightColor, SHADOW_HEIGHT);
// vector for the lights over the pepes since t
vector<PointLight*> pepeLights = { &pepeLight1,&pepeLight2,&pepeLight3,&pepeLight4,&pepeLight5,&pepeLight6 };


PointLight mainLight = PointLight(shapeModel.POS + vec3(0.0f, 10.0f, -5.0f), 200.0f, 1.0f, 0.007f, 0.002f, vec3(0.5f, 1.0f, 0.25f), SHADOW_HEIGHT);
Spotlight spotLight1 = Spotlight(shapeModel.POS + vec3(0.0f, 0.0f, -5.0f), vec3(0.0f, -1.0f, -0.5f), vec3(1.0f), radians(5.0f), radians(18.0f));


//////////////////////////////////////////////// CAMERA ////////////////////////////////////////////////
Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 10.0f, 5.0f), 90.0f);
vec3 cameraPositionBias = vec3(0.0f); // so we can give a bias to the camera around the shape


//////////////////////////////////////////////// SOUND ENGINE ////////////////////////////////////////////////
irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();


//////////////////////////////////////////////// GAME CONSTANTS ////////////////////////////////////////////////
int score = 0; // total score
int highScore = 0;
float totalTime = 60.0f; // total time to get a high score
float gameLastStartTime;
float timeSinceLastPassed; // time since shape last passed wall to get time component of the score
bool flickerScore = false; // tell program we want to flicker the score
bool explosionOccuring = false; // tells program we want to have the explosion effect
bool timeWarningGiven = false; // ensures we only give one time warning

bool enableShadows = true; // rendering flag
bool enableTextures = true; // rendering flag

bool gameRunning = true; // whether or not we still have time in the game
bool shapeRotating = false; // flag whether or not the shape is currently rotating

// placed in case we want to cycle through light colors in SUPERHYPERCUBE game
vector<vec3> lightColors = {
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

vector<char*> successSounds = {
    "../Assets/Sounds/Success1.wav",
    "../Assets/Sounds/Success2.wav",
    "../Assets/Sounds/Success3.wav",
    "../Assets/Sounds/Success4.wav",
    "../Assets/Sounds/Success5.wav",
    "../Assets/Sounds/Success6.wav",
    "../Assets/Sounds/Success7.wav"
};

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
    glClearColor(0.5f * 0.4f, 0.0f, 0.125f * 0.4f, 1.0f);

    //get shader programs
    GLuint sceneShaderProgram = compileAndLinkShaders("../Assets/Shaders/vertexshader.glsl", "../Assets/Shaders/fragmentshader.glsl");
    GLuint shadowShaderProgram = compileAndLinkShaders("../Assets/Shaders/shadowvertexshader.glsl", "../Assets/Shaders/shadowgeometryshader.glsl", "../Assets/Shaders/shadowfragmentshader.glsl");
    GLuint textShaderProgram = compileAndLinkShaders("../Assets/Shaders/textvertexshader.glsl", "../Assets/Shaders/textfragmentshader.glsl");

    // creation of the depth map framebuffer and texture [cube map is used since this is a point light and light is in 360 degrees around it]
    GLuint depthMapFBO, depthCubeMap;
    getShadowCubeMap(&depthMapFBO, &depthCubeMap);

    // make all the models
    initializeModels();

    // make direction of the spotlight
    spotLight1.direction = -normalize(spotLight1.POS - wallModel.POS);

    // seed random number generator
    srand(time(NULL)); 

    //make the textures point to the right position
    glUseProgram(sceneShaderProgram);
    glUniform1i(glGetUniformLocation(sceneShaderProgram, "modelTexture"), 0);
    glUniform1i(glGetUniformLocation(sceneShaderProgram, "shadowMap"), 1);

    // enable openGL effects
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // play music
    soundEngine->play2D("../Assets/Sounds/Breakout.mp3", true); // music courtesy of https://learnopengl.com

    // For frame time
    float lastFrameTime = glfwGetTime();

    // making text renderer objects with flickering effect
    vec3 scoreBaseColor(1.0f);
    vec3 scoreFlashColor(1.0f, 0.0f, 0.0f);
    stringFlickeringEngine scoreTextEngine = stringFlickeringEngine(scoreBaseColor, scoreFlashColor, 0.05f, 10); // creation of score flicker effect
    stringFlickeringEngine timeTextEngine = stringFlickeringEngine(scoreBaseColor, scoreFlashColor, 0.5f, 20); // creation of text flicker effect

    // for scoring the time aspect of the score
    timeSinceLastPassed = lastFrameTime;
    gameLastStartTime = lastFrameTime;
    
    // for explosion
    float curExplosionTime = 0.0f;
    float lengthOfExplosion = 0.35f;
    float lightIntensityFactor = 12.5f;
    vec3 lightInitialColor = mainLight.color;

    shapeModel.rotationQuat = generateStartingAngle();

    //Main loop
    while (!glfwWindowShouldClose(window)) {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

		// window size callback called when window size changes
		glfwSetWindowSizeCallback(window, window_size_callback);


        ////////////////////////////////// GENERATE SHADOW MAP //////////////////////////////////
        // render the depth map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); // change view to the size of the shadow texture
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); // bind the framebuffer
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shadowShaderProgram); // use proper shaders
        // update values in shadow shader
        mainLight.updateShadowShader(shadowShaderProgram);
        renderScene(shadowShaderProgram); // render to make the texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind depth map FBO


        ////////////////////////////////// EXPLOSION EFFECT //////////////////////////////////
        if (explosionOccuring) {
            if (curExplosionTime >= lengthOfExplosion) {// end explosion
                explosionOccuring = false;
                curExplosionTime = 0.0f;
                mainLight.color = lightInitialColor;
            }
            else {
                mainLight.color = (1 + (lightIntensityFactor * dt)) * mainLight.color;
                curExplosionTime += dt;
            }
        }


        ////////////////////////////////// RENDER SCENE //////////////////////////////////
        // render the scene as normal with the shadow mapping using the depth map
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // reset viewport tot hte size of the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(sceneShaderProgram);
        // update the values in the scene shader
        camera.createMatrices(0.01f, 200.0f, sceneShaderProgram, WINDOW_WIDTH, WINDOW_HEIGHT);
        mainLight.updateSceneShader(sceneShaderProgram, "pointlight1", enableShadows);
        spotLight1.updateSceneShader(sceneShaderProgram, "spotlight1");
        // update all the pepe lights
        int pepeNum = 1;
        for (PointLight *pepelight : pepeLights) {
            pepelight->updateSceneShader(sceneShaderProgram, "lightPepe" + to_string(pepeNum++), enableShadows);
        }
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
        renderScene(sceneShaderProgram);


        ////////////////////////////////// DRAW TEXT ////////////////////////////////
        // play effects when time is running out
        if (totalTime - (lastFrameTime - gameLastStartTime) < 10 && !timeWarningGiven) {
            timeTextEngine.drawText(true, "Time Left: \n" + to_string((int)(totalTime - lastFrameTime)), timeTextPosition, 0.01f, textShaderProgram);
            timeWarningGiven = true;
            soundEngine->play2D("../Assets/Sounds/running_out_of_time.wav", false); // from https://freesound.org/people/acclivity/sounds/32243/
        }
        else {
            if(gameRunning)
                timeTextEngine.drawText(false, "Time Left: \n" + to_string((int)(totalTime - (lastFrameTime - gameLastStartTime))), timeTextPosition, 0.01f, textShaderProgram);
            else
                timeTextEngine.drawText(false, "Time Left: \n0", timeTextPosition, 0.01f, textShaderProgram);
        }


        ////////////////////////////////// GAME TIME EVNETS //////////////////////////////////
        if (gameRunning) {
            // bind camera to object
            camera.position = shapeModel.POS + vec3(0.0f, 4.5f, -8.0f) + cameraPositionBias;
            camera.orientation = normalize(shapeModel.POS - camera.position);

            // send textEngine a flicker signal when the player scores
            if (flickerScore) { // start flickering the text
                scoreTextEngine.drawText(true, "Score: \n" + to_string(score) + "\nHigh Score: \n" + to_string(highScore), scoreTextPosition, 0.01f, textShaderProgram);
                flickerScore = false;
            }
            else // draw normally
                scoreTextEngine.drawText("Score: \n" + to_string(score) + "\nHigh Score: \n" + to_string(highScore), scoreTextPosition, 0.01f, textShaderProgram);

            if (totalTime - (lastFrameTime - gameLastStartTime) < 0)
                endGame();
        }
        else { // what to render when the game is done running (the end screen)
            // change draw size and position when the game is over
            scoreTextEngine.drawText(true, "Score: \n" + to_string(score) + "\nHigh Score: \n" + to_string(highScore) + "\nRestart? (Y/N)", vec3(-0.85f, 0.35f, 0.0f), 0.01f * 3, textShaderProgram);

            // rotate pepes when game is done on end screen
            float rateOfRotation = 120.0f;
            pepeModel1.rotationQuat *= angleAxis(radians(rateOfRotation * dt), vec3(0.0f, 1.0f, 0.0f));
            pepeModel3.rotationQuat *= angleAxis(-radians(rateOfRotation * dt), vec3(0.0f, 1.0f, 0.0f));
            pepeModel5.rotationQuat *= angleAxis(radians(rateOfRotation * dt), vec3(0.0f, 1.0f, 0.0f));
        }

        //end frame
        glfwSwapBuffers(window); //swap the front buffer with back buffer
        glfwPollEvents(); //get inputs

        // get inputs
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
    static bool BLastReleased = true, XLastReleased = true, SpaceLastReleased = true;
    float rotationFactor = 5.0f;
    static float modelMovementSpeed = 1.0f;
    float slowMovementSpeed = 2.0f;
    float fastMovementSpeed = 10.0f;

    //variables for game
    static float currentDeg = 0.0f; // tracks how much has been rotated 
    float goalDeg = 90.0f; // how big a rotation will be
    float rateOfRotation = 270.0f; // how fast the model will rotate
    static vec3 rotationAxis = vec3(1.0f, 0.0f, 0.0f);

    camera.processInputs(window, dt); // processes all camera inputs

    if (gameRunning) {

        // make shape go towards wall
        shapeModel.POS += vec3(0.0f, 0.0f, modelMovementSpeed * dt);

        if (shapeModel.POS.z > 0.0f) {
            modelMovementSpeed = slowMovementSpeed;
            shapePassedWall(); // called when we need a reset and a new model
        }



        if (shapeRotating) {
            if (currentDeg < goalDeg) { // continue rotation loop
                // if else checks if we are allowed to increase the rotation by the full allowed amount or not
                if (currentDeg + rateOfRotation * dt > goalDeg)
                    shapeModel.rotationQuat = angleAxis(radians(goalDeg - currentDeg), rotationAxis) * shapeModel.rotationQuat;
                else
                    shapeModel.rotationQuat = angleAxis(radians(rateOfRotation * dt), rotationAxis) * shapeModel.rotationQuat;
            
                currentDeg += rateOfRotation * dt;
            }
            else { // end rotation loop
                // reset flags for next rotation
                currentDeg = 0.0f;
                shapeRotating = false;
            }
        }
        else{
            currentDeg = 0.0f;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                shapeRotating = true;
                rotationAxis = vec3(1.0f, 0.0f, 0.0f); // direction of rotation
            }
 
            else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                shapeRotating = true;
                rotationAxis = -vec3(1.0f, 0.0f, 0.0f); // direction of rotation
            }
            else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                shapeRotating = true;
                rotationAxis = vec3(0.0f, 1.0f, 0.0f); // direction of rotation
            }
            else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                shapeRotating = true;
                rotationAxis = -vec3(0.0f, 1.0f, 0.0f); // direction of rotation
            }
            else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
                shapeRotating = true;
                rotationAxis = -vec3(0.0f, 0.0f, 1.0f); // direction of rotation
            }
            else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
                shapeRotating = true;
                rotationAxis = vec3(0.0f, 0.0f, 1.0f); // direction of rotation
            }
        } 

        // speed up model
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
            SpaceLastReleased = true;
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && SpaceLastReleased) {
            modelMovementSpeed = fastMovementSpeed;
            SpaceLastReleased = false;
        }

        // for camera movement
        static bool cameraMoving = false;
        static vec3 initialBias;
        static float overallCameraMovement = 0.0f;
        static vec3 movementDirection;
        static string cameraPositionX = "center";
        static string cameraPositionY = "center";
        float cameraMoveSpeed = 6.0f;
        float desiredMovement = 2.0f;
        

        if (cameraMoving) {
            if (overallCameraMovement >= desiredMovement) { // we finish moving
                cameraPositionBias = initialBias + (desiredMovement * movementDirection); // snap into the proper position
                overallCameraMovement = 0.0f; // reset counter
                cameraMoving = false;
            }
            else { // we continue to move
                cameraPositionBias += cameraMoveSpeed * dt * movementDirection;
                overallCameraMovement += cameraMoveSpeed * dt;
            }
        }
        else {
            initialBias = cameraPositionBias;

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                if (cameraPositionX != "left") { // make sure the camera is not all the way to the left
                    cameraMoving = true;
                    movementDirection = vec3(1.0f, 0.0f, 0.0f);

                    if (cameraPositionX == "right")
                        cameraPositionX = "center";
                    else
                        cameraPositionX = "left";
                }
            }
            else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                if (cameraPositionX != "right") { // make sure the camera is not all the way to the right
                    cameraMoving = true;
                    movementDirection = vec3(-1.0f, 0.0f, 0.0f);

                    if (cameraPositionX == "left")
                        cameraPositionX = "center";
                    else
                        cameraPositionX = "right";
                }
            }
            else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                if (cameraPositionY != "up") { // make sure the camera is not all the way to the top
                    cameraMoving = true;
                    movementDirection = vec3(0.0f, 1.0f, 0.0f);

                    if (cameraPositionY == "down")
                        cameraPositionY = "center";
                    else
                        cameraPositionY = "up";
                }
            }
            else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                if (cameraPositionY != "down") { // make sure the camera is not all the way to the top
                    cameraMoving = true;
                    movementDirection = vec3(0.0f, -1.0f, 0.0f);

                    if (cameraPositionY == "up")
                        cameraPositionY = "center";
                    else
                        cameraPositionY = "down";
                }
            }
        }

    } 
    else {
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            score = 0;
            totalTime = 60.0f;
            gameLastStartTime = glfwGetTime();
            timeWarningGiven = false;
            shapePassedWall();
            gameRunning = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
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

    // close the window on escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}

void renderScene(GLuint shaderProgram) {

    for (Model *pepe : pepeModels)
        pepe->render(shaderProgram, enableTextures);

    wallModel.render(shaderProgram, enableTextures);

    shapeModel.render(shaderProgram, enableTextures);

    GroundFloor.render(shaderProgram, enableTextures);

}

void initializeModels() {
    // get VAOs
    GLuint cubeModelVAO = getCubeModel();

    // generate the textures needed
    GLuint blankTexture = loadTexture("../Assets/Textures/blank.jpg");
    GLuint metalTexture = loadTexture("../Assets/Textures/wood_texture.jpg"); // from https://www.filterforge.com/filters/9452.jpg
    GLuint explosiveTexture = loadTexture("../Assets/Textures/explosives.jpg"); // from https://i.pinimg.com/236x/52/27/9f/52279f962d19968863ab1448fa973466.jpg
    GLuint dirtTexture = loadTexture("../Assets/Textures/dirt.jpg"); // from http://1.bp.blogspot.com/-dXMlsHE-rUI/UbWXQcc8aVI/AAAAAAAAEHw/fHwfk_zjVNQ/s1600/Seamless+ground+dirt+texture.jpg
    GLuint whiteTex = loadTexture("../Assets/Textures/WhiteTex2.jpg"); // from https://seamless-pixels.blogspot.com/2012/07/seamless-wall-white-paint-stucco.html

    // initialize Materials
    vec3 goldVec(0.780392f * 1.5f, 0.568627f * 1.5f, 0.113725f * 1.5f);
    Material goldMaterial(goldVec, 1.0f);
    Material brickMaterial = Material(vec3(1.0f), 0.01f);
    Material tileMaterial = Material(vec3(1.0f), 0.2f);

    shapeModel.setMaterial(brickMaterial);
    shapeModel.linkVAO(cubeModelVAO, 36);
    shapeModel.linkTexture(explosiveTexture);

    wallModel.setMaterial(brickMaterial);
    wallModel.linkVAO(cubeModelVAO, 36);
    wallModel.linkTexture(metalTexture);

    int pepeVertices;
    Material pepeMaterial = Material(vec3((float)43 / 255, (float)106 / 255, (float)64 / 255), 0.2f);
    GLuint pepeVAO = setupModelVBO("../Assets/Models/Pepe.obj", pepeVertices);
    bool oddPepe = true;

    for (Model *pepe : pepeModels) {
        pepe->linkVAO(pepeVAO, pepeVertices);
        pepe->setMaterial(pepeMaterial);
        pepe->linkTexture(whiteTex);

        if (oddPepe)
            pepe->rotationQuat = angleAxis(radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * pepe->rotationQuat;

        oddPepe = !oddPepe;
    }

    GroundFloor.linkVAO(cubeModelVAO, 36);
    GroundFloor.texWrapX = 8.0f;
    GroundFloor.texWrapY = 8.0f;
    GroundFloor.linkTexture(dirtTexture);
    GroundFloor.setMaterial(tileMaterial);
}

void shapePassedWall() {
    // scoring factors
    int scoreForPassingWall = 100;
    int timeScoreBonus = 300;
    float timeBonusFactor = 0.9;

    // keeps track of what announcer track to play
    static int announcerIndex = 0;

    if (gameRunning) {
        // check if the shape was the correct orientation
        float bias = radians(1.0f); // degrees off of a pure 0 deg due to floating point errors
        bool passedThroughWall = true;
        vec3 endingOrientation = eulerAngles(shapeModel.rotationQuat); // convert the quarternion to euler angles to make easier to conceptualize

        if (!(endingOrientation.x > -bias && endingOrientation.x < bias)) // check x component
            passedThroughWall = false;
        if (!(endingOrientation.y > -bias && endingOrientation.y < bias)) // check y component
            passedThroughWall = false;
        if (!(endingOrientation.z > -bias && endingOrientation.z < bias)) // check z component
            passedThroughWall = false;

        if (passedThroughWall) { // events happening if the shape successfully passes the wall
            soundEngine->play2D(successSounds[announcerIndex++ % successSounds.size()]); // playing success sound

            // score calculations
            int scoreToAdd = scoreForPassingWall + (int)(timeScoreBonus * pow(timeBonusFactor, glfwGetTime() - timeSinceLastPassed));
            score += scoreToAdd;
            highScore = score > highScore ? score : highScore;
            totalTime += (score <= 1000 ? 5.0f : (5000.0f / (float)score));

            flickerScore = true; // flag to begin score flash effect in the main loop
        }
        else { // events that happen if the shape fails to go through the wall
            soundEngine->play2D("../Assets/Sounds/explosion.wav"); // from freesound.org

            explosionOccuring = true;
        }
    }


    timeSinceLastPassed = glfwGetTime(); // used for scoring the time component

    shapeRotating = false; // stops errors that occur from the shape being in mid rotation when it passes the wall

	shapeModel.resetModel(); // brings shape to intial position
    shapeModel.rotationQuat = generateStartingAngle(); // creates a random orientation for the new shape

    int filePathIndex = rand() % shapePaths.size();
    while(shapeModel.getFilePath() == shapePaths[filePathIndex]) // ensure past shape is not the same as the new one
        filePathIndex = rand() % shapePaths.size();

	shapeModel.updateFilePath(shapePaths[filePathIndex]); // change the shape

	wallModel.updateFilePath(buildWall(shapeModel.getFilePath())); // update the wall to correspond to the new shape

    // change the main light for dramatic effect
    mainLight.color = lightColors[rand() % lightColors.size()];
}

void endGame() {
    // handles the events to occur at the end of the game
    shapeModel.resetModel();
    gameRunning = false;

    // ominous coloring
    mainLight.color = vec3(0.0f, 0.5f , 0.0f); 
    for (PointLight *pepeLight : pepeLights) {
        pepeLight->color = vec3(0.85f, 0.25f, 0.0f);
    }

    // set the scene for ending
    cameraPositionBias = vec3(0.0f);
    camera.position = pepeModel3.POS + vec3(-7.5f, 0.0f, 0.0f);
    camera.orientation = normalize(pepeModel3.POS - camera.position);

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

quat generateStartingAngle() {
    float possibleAngles[] = { 0.0f, 90.0f, 180.0f, 270.0f };
    int arraySize = 4;
    return quat(vec3(radians(possibleAngles[rand() % arraySize]), radians(possibleAngles[rand() % arraySize]), radians(possibleAngles[rand() % arraySize])));
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
