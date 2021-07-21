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

using namespace std;

//dimensions of the window in pixels
const int width = 1024;
const int height = 768;

const float FOV = 90.0f; // FOV of the player view in degrees

string currentObject = "Jack";

//initialize model scales, degree rotation, filepaths and drawmodes
GLfloat JackScale = 1.0f;
GLfloat MelScale = 1.0f;
GLfloat CedrikScale = 1.0f;
GLfloat AlexScale = 1.0f;

GLfloat JackRotation = 0.0f;
GLfloat MelRotation = 0.0f;
GLfloat CedrikRotation = 0.0f;
GLfloat AlexRotation = 0.0f;

GLenum JackDrawMode = GL_TRIANGLES;
GLenum MelDrawMode = GL_TRIANGLES;
GLenum CedrikDrawMode = GL_TRIANGLES;
GLenum AlexDrawMode = GL_TRIANGLES;

string JacksShape = "../Assets/Shapes/Jack's Shape.csv";
string MelShape = "../Assets/Shapes/MelShape.csv";
string CedriksShape = "../Assets/Shapes/Cedrik's Shape.csv";
string AlexsShape = "../Assets/Shapes/Alex's Shape.csv";

glm::vec3 JackPOS = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 MelPOS = glm::vec3(75.0f, 0.0f, 75.0f);
glm::vec3 CedrikPOS = glm::vec3(75.0f, 0.0f, -75.0f);
glm::vec3 AlexPOS = glm::vec3(-75.0f, 0.0f, 75.0f);


char* readFile(const char* filePath);

char* getVertexShaderSource() { return readFile("../Assets/Shaders/vertexShader.glsl"); }

char* getFragmentShaderSource() { return readFile("../Assets/Shaders/fragmentShader.glsl"); }

GLuint getCubeModel(glm::vec3 color);

GLuint getGridModel(glm::vec3 color);

GLuint compileAndLinkShaders();

void renderShapeFromCSV(string filePath, glm::vec3 pos, GLfloat scale, GLenum drawMode, GLfloat degrees, GLuint shaderProgram);

void renderGrid(GLuint shaderProgram);

void executeEvents(GLFWwindow* window, Camera& camera, float dt);

void renderLine(glm::vec3 pos, glm::vec3 size, glm::vec3 color, GLfloat scale, GLuint shaderProgram);

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
    GLFWwindow* window = glfwCreateWindow(width, height, "Comp371 - Assignment 1", NULL, NULL);
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
    GLuint shaderProgram = compileAndLinkShaders();


    //get VAOs
    GLuint shapeVAO = getCubeModel(glm::vec3(1.0f, 0.0f, 0.0f));
    GLuint wallVAO = getCubeModel(glm::vec3(0.8f, 0.8f, 0.8f));
    GLuint gridVAO = getGridModel(glm::vec3(1.0f, 1.0f, 0.0f));

    //generate camera
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    // For frame time
    float lastFrameTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //Main loop
    while (!glfwWindowShouldClose(window)) {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the window each frame
        glUseProgram(shaderProgram);

        camera.createMatrices(FOV, 0.01f, 100.0f, shaderProgram); // takes care of the view and projection matrices

        glBindVertexArray(shapeVAO);
        renderShapeFromCSV(JacksShape, JackPOS, JackScale, JackDrawMode, JackRotation, shaderProgram);
        renderShapeFromCSV(MelShape, MelPOS, MelScale, MelDrawMode, MelRotation, shaderProgram);
        renderShapeFromCSV(CedriksShape, CedrikPOS, CedrikScale, CedrikDrawMode, CedrikRotation, shaderProgram);
        renderShapeFromCSV(AlexsShape, AlexPOS, AlexScale, AlexDrawMode, AlexRotation, shaderProgram);

        glBindVertexArray(0);

        glBindVertexArray(wallVAO);

        renderShapeFromCSV("../Assets/Shapes/Jack's Wall.csv", JackPOS + glm::vec3(0.0f, 0.0f, 10.0f), JackScale, JackDrawMode, 0.0f, shaderProgram);
        renderShapeFromCSV("../Assets/Shapes/MelWall.csv", MelPOS + glm::vec3(0.0f, 0.0f, 10.0f), MelScale, MelDrawMode, 0.0f, shaderProgram);
        renderShapeFromCSV("../Assets/Shapes/Cedrik's Wall.csv", CedrikPOS + glm::vec3(0.0f, 0.0f, 10.0f), CedrikScale, CedrikDrawMode, 0.0f, shaderProgram);
        renderShapeFromCSV("../Assets/Shapes/Alex's Wall.csv", AlexPOS + glm::vec3(0.0f, 0.0f, 10.0f), AlexScale, AlexDrawMode, 0.0f, shaderProgram);

        glBindVertexArray(0);

        glBindVertexArray(gridVAO);

        renderGrid(shaderProgram);

        glBindVertexArray(0);

        //render the origin lines
        renderLine(glm::vec3(0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, shaderProgram); // x direction
        renderLine(glm::vec3(0.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, shaderProgram); // y direction
        renderLine(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, shaderProgram); // z direction

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
    static bool JLastStateReleased = true;

    camera.processInputs(window, dt); // processes all camera inputs

    //change object
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) 
        currentObject = "Jack";
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        currentObject = "Mel";
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        currentObject = "Cedrik";
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        currentObject = "Alex";





    // size up function
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if(currentObject == "Jack")
            JackScale *= (dt * (float)(8 / 7)); // exponential resizing
        else if (currentObject == "Mel")
            MelScale *= (dt * (float)(8 / 7));
        else if (currentObject == "Cedrik")
            CedrikScale *= (dt * (float)(8 / 7));
        else if (currentObject == "Alex")
            AlexScale *= (dt * (float)(8 / 7));
    }
  

    // size down function
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (currentObject == "Jack")
            JackScale *= (dt * (float)(7 / 8)); // exponential resizing
        else if (currentObject == "Mel")
            MelScale *= (dt * (float)(7 / 8));
        else if (currentObject == "Cedrik")
            CedrikScale *= (dt * (float)(7 / 8));
        else if (currentObject == "Alex")
            AlexScale *= (dt * (float)(7 / 8));
    }


    // change the draw mode
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (currentObject == "Jack")
            JackDrawMode = GL_TRIANGLES;
        else if (currentObject == "Mel")
            MelDrawMode = GL_TRIANGLES;
        else if (currentObject == "Cedrik")
            CedrikDrawMode = GL_TRIANGLES;
        else if (currentObject == "Alex")
            AlexDrawMode = GL_TRIANGLES;
    }
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (currentObject == "Jack")
            JackDrawMode = GL_LINES;
        else if (currentObject == "Mel")
            MelDrawMode = GL_LINES;
        else if (currentObject == "Cedrik")
            CedrikDrawMode = GL_LINES;
        else if (currentObject == "Alex")
            AlexDrawMode = GL_LINES;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (currentObject == "Jack")
            JackDrawMode = GL_POINTS;
        else if (currentObject == "Mel")
            MelDrawMode = GL_POINTS;
        else if (currentObject == "Cedrik")
            CedrikDrawMode = GL_POINTS;
        else if (currentObject == "Alex")
            AlexDrawMode = GL_POINTS;
    }



    static bool PeriodLastStateReleased = true;


    // shuffle the shape function
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_RELEASE) 
        PeriodLastStateReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS && PeriodLastStateReleased == true) {
        if (currentObject == "Jack") {
            if (JacksShape == "../Assets/Shapes/Jack's Shape.csv")
                JacksShape = "../Assets/Shapes/Jack's Shape - Shuffle 1.csv";
            else if (JacksShape == "../Assets/Shapes/Jack's Shape - Shuffle 1.csv")
                JacksShape = "../Assets/Shapes/Jack's Shape - Shuffle 2.csv";
            else
                JacksShape = "../Assets/Shapes/Jack's Shape.csv";
        }
        else if (currentObject == "Mel") {
            if (MelShape == "../Assets/Shapes/MelShape.csv")
                MelShape = "../Assets/Shapes/MelShape-Shuffle.csv";
            else
                MelShape = "../Assets/Shapes/MelShape.csv";
        }
        else if (currentObject == "Cedrik") {
            if (CedriksShape == "../Assets/Shapes/Cedrik's Shape.csv")
                CedriksShape = "../Assets/Shapes/Cedrik's Shape - Shuffle 1.csv";
            else
                CedriksShape = "../Assets/Shapes/Cedrik's Shape.csv";
        }
        else if (currentObject == "Alex") {
            if (AlexsShape == "../Assets/Shapes/Alex's Shape.csv")
                AlexsShape = "../Assets/Shapes/Alex's Shape - Shuffle 1.csv";
            else
                AlexsShape = "../Assets/Shapes/Alex's Shape.csv";
        }

        PeriodLastStateReleased = false;
    }

}

void renderShapeFromCSV(string filePath, glm::vec3 pos, GLfloat scale, GLenum drawMode, GLfloat degrees, GLuint shaderProgram) {
    /** Renders the shape specified in the CSV file provided by using transformed cube models. A cube model should be binded before this method is called.
    *
    *   filePath - filePath of the .csv file that defines the shape
    *   pos - coordinates of the object
    *   scale - how much the object is to be scaled by
    *   drawMode - renderer mode
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
    bool cubeProcessed;
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

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

            glm::vec3 positionVec = glm::rotateY(glm::vec3(cubeInfo[0], cubeInfo[1], cubeInfo[2]), glm::radians(degrees)); // convert the local positions to the proper degrees
            //now we draw the cube
            glm::mat4 cubeWorldMatrix = glm::translate(glm::mat4(1.0), positionVec + pos) * glm::rotate(glm::mat4(1.0f), glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(cubeInfo[3], cubeInfo[4], cubeInfo[5]));
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

GLuint compileAndLinkShaders() {
    /* compile and link shader program
    * return shader program id
    */

    // create vertex shader
    const char* vertexShaderSource = getVertexShaderSource();
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
    const char* fragmentShaderSource = getFragmentShaderSource();
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

void renderGrid(GLuint shaderProgram) {

    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

    // Draw Grid
    glm::mat4 pillarWorldMatrix = glm::mat4(1.0f);
    for (int i = 0; i < 100; ++i)
    {
        for (int j = 0; j < 100; ++j)
        {
            pillarWorldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f + i * 1.0f, -10.0f, -50.0f + j * 1.0f));
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
    glm::vec3 vertexArray[] = {  // position,                            color
        glm::vec3(-0.5f,-0.5f,-0.5f), color, // left
        glm::vec3(-0.5f,-0.5f, 0.5f), color,
        glm::vec3(-0.5f, 0.5f, 0.5f), color,

        glm::vec3(-0.5f,-0.5f,-0.5f), color,
        glm::vec3(-0.5f, 0.5f, 0.5f), color,
        glm::vec3(-0.5f, 0.5f,-0.5f), color,

        glm::vec3(0.5f, 0.5f,-0.5f), color, // far
        glm::vec3(-0.5f,-0.5f,-0.5f), color,
        glm::vec3(-0.5f, 0.5f,-0.5f), color,

        glm::vec3(0.5f, 0.5f,-0.5f), color,
        glm::vec3(0.5f,-0.5f,-0.5f), color,
        glm::vec3(-0.5f,-0.5f,-0.5f), color,

        glm::vec3(0.5f,-0.5f, 0.5f), color, // bottom
        glm::vec3(-0.5f,-0.5f,-0.5f), color,
        glm::vec3(0.5f,-0.5f,-0.5f), color,

        glm::vec3(0.5f,-0.5f, 0.5f), color,
        glm::vec3(-0.5f,-0.5f, 0.5f), color,
        glm::vec3(-0.5f,-0.5f,-0.5f), color,

        glm::vec3(-0.5f, 0.5f, 0.5f), color, // near
        glm::vec3(-0.5f,-0.5f, 0.5f), color,
        glm::vec3(0.5f,-0.5f, 0.5f), color,

        glm::vec3(0.5f, 0.5f, 0.5f), color,
        glm::vec3(-0.5f, 0.5f, 0.5f), color,
        glm::vec3(0.5f,-0.5f, 0.5f), color,

        glm::vec3(0.5f, 0.5f, 0.5f), color, // right
        glm::vec3(0.5f,-0.5f,-0.5f), color,
        glm::vec3(0.5f, 0.5f,-0.5f), color,

        glm::vec3(0.5f,-0.5f,-0.5f), color,
        glm::vec3(0.5f, 0.5f, 0.5f), color,
        glm::vec3(0.5f,-0.5f, 0.5f), color,

        glm::vec3(0.5f, 0.5f, 0.5f), color, // top
        glm::vec3(0.5f, 0.5f,-0.5f), color,
        glm::vec3(-0.5f, 0.5f,-0.5f), color,

        glm::vec3(0.5f, 0.5f, 0.5f), color,
        glm::vec3(-0.5f, 0.5f,-0.5f), color,
        glm::vec3(-0.5f, 0.5f, 0.5f), color,
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

char* readFile(const char* filePath) { //credit: https://badvertex.com/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c.html
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