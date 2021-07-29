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
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace glm;

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv, vec3 _normal)
        : position(_position), color(_color), uv(_uv), normal(_normal) {}

    vec3 position;
    vec3 color;
    vec2 uv;
    vec3 normal;
};

char* readFile(string filePath);

char* getVertexShaderSource(string vertexShaderFilePath) { return readFile(vertexShaderFilePath); }

char* getFragmentShaderSource(string fragmentShaderFilePath) { return readFile(fragmentShaderFilePath); }

GLuint getCubeModel(glm::vec3 color);

GLuint getGridModel(glm::vec3 color);

GLuint compileAndLinkShaders(string vertexShaderFilePath, string fragmentShaderFilePath);

GLuint compileAndLinkShaders(string vertexShaderFilePath, string geometryShaderFilePath, string fragmentShaderFilePath);

void getShadowDepthMap(GLuint *frameBufferPtr, GLuint *texturePtr);

void getShadowCubeMap(GLuint* frameBufferPtr, GLuint* texturePtr);

void renderShapeFromCSV(string filePath, glm::vec3 pos, GLfloat scale, GLenum drawMode, glm::vec3 rotationalVector, GLuint shaderProgram);

void renderGrid(GLuint shaderProgram);

void executeEvents(GLFWwindow* window, Camera& camera, float dt);

void renderLine(glm::vec3 pos, glm::vec3 size, glm::vec3 color, GLfloat scale, GLuint shaderProgram);

GLuint loadTexture(const char* filename);

void renderScene(GLuint shapeVAO, GLuint wallVAO, GLuint gridVAO, GLuint shaderProgram);

// dimensions of the window in pixels
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

// dimensions of the shadow 
const int SHADOW_WIDTH = 1024;
const int SHADOW_HEIGHT = 1024;

const float initialFOV = 90.0f; // FOV of the initial player view in degrees
const GLfloat initialScale = 1.0f; // initial object scale
const glm::vec3 initialRotationVector = glm::vec3(0.0f);
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

//creation of model objects to remove switch statements in the executeEvents method
Model JacksModel = Model(JacksShape, 3, JackInitialPOS, initialScale, GL_TRIANGLES);
Model MelModel = Model(MelShape, 2, MelInitialPOS, initialScale, GL_TRIANGLES);
Model CedriksModel = Model(CedriksShape, 2, CedrikInitialPOS, initialScale, GL_TRIANGLES);
Model AlexsModel = Model(AlexsShape, 2, AlexInitialPOS, initialScale, GL_TRIANGLES);
Model ThapansModel = Model(ThapansShape, 2, ThapanInitialPOS, initialScale, GL_TRIANGLES);

Model *currentObject = &JacksModel; // we use a pointer to the current object to do object manipulations


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

    //get shader program
    GLuint sceneShaderProgram = compileAndLinkShaders("../Assets/Shaders/vertexshader.glsl", "../Assets/Shaders/fragmentshader.glsl");
    //GLuint shadowShaderProgram = compileAndLinkShaders("../Assets/Shaders/shadowvertexshader.glsl", "../Assets/Shaders/shadowfragmentshader.glsl");
    GLuint shadowShaderProgram = compileAndLinkShaders("../Assets/Shaders/shadowvertexshader.glsl", "../Assets/Shaders/shadowgeometryshader.glsl", "../Assets/Shaders/shadowfragmentshader.glsl");

    GLuint shadowDepthMapFBO, shadowDepthMapTexture;
    getShadowCubeMap(&shadowDepthMapFBO, &shadowDepthMapTexture);

    // get VAOs
    GLuint shapeVAO = getCubeModel(glm::vec3(1.0f, 0.0f, 0.0f));
    GLuint wallVAO = getCubeModel(glm::vec3(0.8f, 0.8f, 0.8f));
    GLuint gridVAO = getGridModel(glm::vec3(1.0f, 1.0f, 0.0f));
    GLuint lightVAO = getCubeModel(vec3(1.0f, 1.0f, 1.0f));

    // generate the textures needed
    GLuint brickTexture = loadTexture("../Assets/Textures/brick.jpg");
    GLuint blankTexture = loadTexture("../Assets/Textures/blank.jpg");

    //generate camera
    Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 10.0f, 5.0f), initialFOV);

    //generate light
    PointLight light(vec3(0.0f, 30.0f, 0.0f), 90.0f, 1.0f, 0.007f, 0.0002f, vec3(0.0f, -1.0f, 0.0f), vec3(1.0f), 1024);

    // For frame time
    float lastFrameTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //Main loop
    while (!glfwWindowShouldClose(window)) {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        // update values in shadow shader
        light.updateShadowShader(shadowShaderProgram); 
        //update the values in the scene shader
        camera.createMatrices(0.01f, 100.0f, sceneShaderProgram);
        light.updateSceneShader(sceneShaderProgram);


        // render the depth map
        glUseProgram(shadowShaderProgram); // use proper shaders
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); // change view to the size of the shadow texture
        glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthMapFBO); // bind the framebuffer
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(shapeVAO, wallVAO, gridVAO, shadowShaderProgram); // render to make the texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind depth map FBO

        // render the scene as normal with the shadow mapping using the depth map
        glUseProgram(sceneShaderProgram);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // reset viewport tot hte size of the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        glBindTexture(GL_TEXTURE_2D, brickTexture);
      //  glBindTexture(GL_TEXTURE_CUBE_MAP, shadowDepthMapTexture);

        renderScene(shapeVAO, wallVAO, gridVAO, sceneShaderProgram);


        glBindTexture(GL_TEXTURE_2D, blankTexture);
        glBindVertexArray(lightVAO);
        mat4 lightWorldMatrix = translate(mat4(1.0f), light.POS);
        glUniformMatrix4fv(glGetUniformLocation(sceneShaderProgram, "worldMatrix"), 1, GL_FALSE, &lightWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
    static bool JLastStateReleased = true, ULastStateReleased = true, PeriodLastStateReleased = true, ALastStateReleased = true, DLastStateReleased = true, SpaceLastStateReleased = true;
    float scaleFactor = (float)8 / 7;
    float rotationFactor = 5.0f;
    float modelMovementSpeed = 2.0f;


    camera.processInputs(window, dt); // processes all camera inputs

    //change object
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) 
        currentObject = &JacksModel;
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        currentObject = &MelModel;
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        currentObject = &CedriksModel;
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        currentObject = &AlexsModel;
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        currentObject = &ThapansModel;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        SpaceLastStateReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && SpaceLastStateReleased) {
        currentObject->resetModel();
        SpaceLastStateReleased = false;
    }


    // size up function
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE)
        ULastStateReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && ULastStateReleased) {
        currentObject->scale *= scaleFactor;
        ULastStateReleased = false;
    }
  

    // size down function
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE)
        JLastStateReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && JLastStateReleased) {
        currentObject->scale *= 1 / scaleFactor;
        JLastStateReleased = false;
    }


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
        ALastStateReleased = true;


    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
        DLastStateReleased = true;


    // movement function
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
            currentObject->POS += glm::vec3(0.0f, modelMovementSpeed * dt, 0.0f);

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
            currentObject->POS += glm::vec3(0.0f, -modelMovementSpeed * dt, 0.0f);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
            currentObject->POS += glm::vec3(-modelMovementSpeed * dt, 0.0f, 0.0f);
           
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
            currentObject->POS += glm::vec3(modelMovementSpeed * dt, 0.0f, 0.0f);

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            currentObject->POS += glm::vec3(0.0f, 0.0f, modelMovementSpeed * dt);

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            currentObject->POS += glm::vec3(0.0f, 0.0f, -modelMovementSpeed * dt);
    }
    else {
        // rotate counter-clockwise around positive y-axis
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && ALastStateReleased) {
            currentObject->rotationVector.y += rotationFactor;
            ALastStateReleased = false;
        }

        // rotate clockwise around positive y-axis
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && DLastStateReleased) {
            currentObject->rotationVector.y -= rotationFactor;
            DLastStateReleased = false;
        }
    }


    // change the draw mode
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        currentObject->drawMode = GL_TRIANGLES;
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        currentObject->drawMode = GL_LINES;
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) 
        currentObject->drawMode = GL_POINTS;


    // shuffle the shape function
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_RELEASE) 
        PeriodLastStateReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS && PeriodLastStateReleased == true) {
        currentObject->shuffle();
        PeriodLastStateReleased = false;
    }

    // close the window on escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}

void renderShapeFromCSV(string filePath, glm::vec3 pos, GLfloat scale, GLenum drawMode, glm::vec3 rotationalVector, GLuint shaderProgram) {
    /** Renders the shape specified in the CSV file provided by using transformed cube models. A cube model should be binded before this method is called.
    *
    *   filePath - filePath of the .csv file that defines the shape
    *   pos - coordinates of the object
    *   scale - how much the object is to be scaled by
    *   drawMode - renderer mode
    *   rotationalVector - how many degrees clockwise around the respective axis to rotate the object around (degrees around x-axis, degrees around y-axis, degrees around z-axis)
    *   shaderProgram - shader program used by the running OpenGL application
    *
    * Each line in the csv file renders a seperate rectangular prism. The line should be formatted as such:
    * local x coordinate, local y coordinate, local z coordinate, local x scale, local y scale, local z scale
    **/

    ifstream fileStream(filePath, ios::in);

    if (!fileStream.is_open()) {
        cerr << "Could not read file " << filePath << ". File does not exist." << endl;
        return;
    }

    string value, line = "";
    float cubeInfo[6] = {};
    int i, j;
    char curChar;
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

    //creation of base matrix
    glm::mat4 baseMatrix = glm::mat4(1.0f);
    baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationalVector.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate around x axis
    baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationalVector.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate around y axis
    baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationalVector.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate around z axis
    baseMatrix = glm::translate(baseMatrix, pos);

    while (!fileStream.eof()) {
        getline(fileStream, line); //get line for a cube
        line.append("\n");

        value = "";
        i = 0; // curChar index
        j = 0; // cubeInfo index

        // parse the line to get cube info in the form of: 
        //(local x coord,       local y coord,      local z coord,      size in x dir,      size in y dir,      size in z dir)
        while (true) {
            curChar = line[i++];

            if (curChar == '\n' || curChar == ',') {//need to push the info when these chars appear
                cubeInfo[j++] = stof(value); // push float value to the array
                value = "";
            }
            else if (curChar != ' ') // ignore spaces
                value.append(string(1, curChar)); // add the char to the current value

             // line is finished
            if (curChar == '\n' || curChar == '/' || j == 6)
                break;
        }

        if (j < 6)  //if we didnt get to 6 then we either have too little values per line
            cout << "There are not enough values to fully define an object" << endl;
        else {
            //scale the values
            for (int k = 0; k < 6; k++) {
                cubeInfo[k] = scale * cubeInfo[k];
            }

            glm::vec3 localCoord = glm::vec3(cubeInfo[0], cubeInfo[1], cubeInfo[2]);
            glm::vec3 scalingVector = glm::vec3(cubeInfo[3], cubeInfo[4], cubeInfo[5]);

            // transformation of the base matrix
            glm::mat4 cubeWorldMatrix = glm::translate(baseMatrix, localCoord);
            cubeWorldMatrix = glm::scale(cubeWorldMatrix, scalingVector);

            // draw the cube
             glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &cubeWorldMatrix[0][0]);



            // change the draw mode of the model being rendered
            if (drawMode == GL_TRIANGLES)
                glDrawArrays(GL_TRIANGLES, 0, 36);
            else if (drawMode == GL_LINES) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // turn on wireframe
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // turn off wireframe
            }
            else if (drawMode == GL_POINTS)
                glDrawArrays(GL_POINTS, 0, 36);
            else
                cerr << "Invalid draw type. Reader supports: GL_TRIANGLES, GL_LINES, GL_POINTS" << endl;

        }
    }
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
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
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

GLuint getCubeModel(glm::vec3 color) {
    /* Creates a cube model VAO with the given color.
    * This method should not be called in any loops as it will create a memory leak. Use before the main program loop
    *
    *   color - color vector of the cube
    *
    * returns the GLuint corresponding with the created VAO
    */

    // Cube model
    TexturedColoredVertex vertexArray[] = {  // position,                            color
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)), //left 
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), color, vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), color, vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), color, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)), // far 
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), color, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), color, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), color, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)), // bottom
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), color, vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), color, vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)), // near
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), color, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), color, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), color, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)), // right
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), color, vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), color, vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)), // top
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), color, vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), color, vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f))
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

    //create color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    //create texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(2*sizeof(vec3)));
    glEnableVertexAttribArray(2);

    //create normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(2 * sizeof(vec3) + sizeof(vec2)));
    glEnableVertexAttribArray(3);

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

void getShadowDepthMap(GLuint *frameBufferPtr, GLuint *texturePtr) {
    
    glGenFramebuffers(1, frameBufferPtr);
    glGenTextures(1, texturePtr);

    glBindTexture(GL_TEXTURE_2D, *texturePtr);

    //generate the shadow texture
    glTexImage2D(GL_TEXTURE_2D, // type of texture
        0,                      // texture level of detail
        GL_DEPTH_COMPONENT,     // internal format of the color components
        SHADOW_WIDTH,           // width of texture
        SHADOW_HEIGHT,          // height of texture
        0,                      // border - reference says this must be 0???
        GL_DEPTH_COMPONENT,     // format of the pixel data
        GL_FLOAT,               // data type of the pixel data
        NULL);                  // pointer to image data

    // telling the texture sampler the desired filtering methods, GL_NEAREST says to use the value of the nearest pixel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // defining how to wrap the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, *frameBufferPtr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *texturePtr, 0);
    // since we only need depth information, we tell frame buffer we dont need a color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
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

void renderScene(GLuint shapeVAO, GLuint wallVAO, GLuint gridVAO, GLuint shaderProgram) {
    glBindVertexArray(shapeVAO);

    renderShapeFromCSV(JacksModel.filePath, JacksModel.POS, JacksModel.scale, JacksModel.drawMode, JacksModel.rotationVector, shaderProgram);
    renderShapeFromCSV(MelModel.filePath, MelModel.POS, MelModel.scale, MelModel.drawMode, MelModel.rotationVector, shaderProgram);
    renderShapeFromCSV(CedriksModel.filePath, CedriksModel.POS, CedriksModel.scale, CedriksModel.drawMode, CedriksModel.rotationVector, shaderProgram);
    renderShapeFromCSV(AlexsModel.filePath, AlexsModel.POS, AlexsModel.scale, AlexsModel.drawMode, AlexsModel.rotationVector, shaderProgram);
    renderShapeFromCSV(ThapansModel.filePath, ThapansModel.POS, ThapansModel.scale, ThapansModel.drawMode, ThapansModel.rotationVector, shaderProgram);

    glBindVertexArray(0);

    glBindVertexArray(wallVAO);

    renderShapeFromCSV("../Assets/Shapes/Jack's Wall.csv", JackInitialPOS + glm::vec3(0.0f, 0.0f, 10.0f), JacksModel.scale, JacksModel.drawMode, initialRotationVector, shaderProgram);
    renderShapeFromCSV("../Assets/Shapes/MelWall.csv", MelInitialPOS + glm::vec3(0.0f, 0.0f, 10.0f), MelModel.scale, MelModel.drawMode, initialRotationVector, shaderProgram);
    renderShapeFromCSV("../Assets/Shapes/Cedrik's Wall.csv", CedrikInitialPOS + glm::vec3(0.0f, 0.0f, 10.0f), CedriksModel.scale, CedriksModel.drawMode, initialRotationVector, shaderProgram);
    renderShapeFromCSV("../Assets/Shapes/Alex's Wall.csv", AlexInitialPOS + glm::vec3(0.0f, 0.0f, 10.0f), AlexsModel.scale, AlexsModel.drawMode, initialRotationVector, shaderProgram);
    renderShapeFromCSV("../Assets/Shapes/Thapan's Wall.csv", ThapanInitialPOS + glm::vec3(0.0f, 0.0f, 10.0f), ThapansModel.scale, ThapansModel.drawMode, initialRotationVector, shaderProgram);


    glBindVertexArray(0);

    glBindVertexArray(gridVAO);

    renderGrid(shaderProgram);

    glBindVertexArray(0);

    //render the origin lines
    //          position                    length                      color             scale
    renderLine(glm::vec3(0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, shaderProgram); // x direction
    renderLine(glm::vec3(0.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, shaderProgram); // y direction
    renderLine(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, shaderProgram); // z direction

}