#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <algorithm>    // added this line to use std::min and std::max
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
float scale = 0.2f; // initial scale of all the models 
GLenum drawMode = GL_TRIANGLES;

string JacksShape = "Jack's Shape.csv";




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

    char *chars = new char[content.length()+1];
    strcpy(chars, content.c_str());

    return chars;
}

char* getVertexShaderSource() { return readFile("vertexShader.glsl"); }

char* getFragmentShaderSource() { return readFile("fragmentShader.glsl"); }

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
    glVertexAttribPointer(0, 3,  GL_FLOAT,  GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    //create color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
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

void renderShapeFromCSV(string filePath, glm::vec3 pos, GLfloat scale, GLenum drawMode, GLuint shaderProgram);

void executeEvents(GLFWwindow* window, Camera &camera, float dt);

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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //get shader program
    GLuint shaderProgram = compileAndLinkShaders();


    //get VAOs - we use two different ones so we can have different colors
    GLuint shapeVAO = getCubeModel(glm::vec3(1.0f, 1.0f, 0.0f));
    GLuint wallVAO = getCubeModel(glm::vec3(0.5f, 0.5f, 0.5f));


    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    float angle = 0;
    float rotationSpeed = 180.0f;  // 180 degrees per second
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

        renderShapeFromCSV(JacksShape, glm::vec3(10.0f, 0.5f, -10.0f), scale, drawMode, shaderProgram);

        glBindVertexArray(0);

        glBindVertexArray(wallVAO);

        renderShapeFromCSV("Jack's Wall.csv", glm::vec3(0.0f, 0.0f, 0.0f), scale, drawMode, shaderProgram);

        glBindVertexArray(0);

        //end frame
        glfwSwapBuffers(window); //swap the front buffer with back buffer
        glfwPollEvents(); //get inputs
        
        executeEvents(window, camera, dt);

        // ROTATION
        angle = (angle + rotationSpeed * dt); // angles in degrees, but glm expects radians (conversion below)
        if (GLFWGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { // Rx: Counter-clockwise rotation about positive x-axis
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
        }
        if (GLFWGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { // R-x: Counter-clockwise rotation about negative x-axis
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(-1.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
        }
        if (GLFWGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {    // Ry: Counter-clockwise rotation about positive y-axis
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
        }
        if (GLFWGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {  // R-y: Counter-clockwise rotation about negative y-axis
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, -1.0f, 0.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
        }
        if (GLFWGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) {  // Reset world position and orientation
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
        }

        // PAN AND TILT
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Convert to spherical coordinates
        const float cameraAngularSpeed = 60.0f;
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
        if (cameraHorizontalAngle > 360)
        {
            cameraHorizontalAngle -= 360;
        }
        else if (cameraHorizontalAngle < -360)
        {
            cameraHorizontalAngle += 360;
        }

        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);

        // PAN in x direction
        while (lastMouseLeftState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            orientation = vec3(cosf(phi) * cosf(theta), 0, 0);  // Pan only in x direction
            vec3 cameraSideVector = glm::cross(orientation, vec3(0.0f, 1.0f, 0.0f));
            glm::normalize(cameraSideVector);
        }
        // TILT in y direction
        while (lastMouseLeftState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            orientation = vec3(0, sinf(phi), 0);    // Tilt only in y direction
            vec3 cameraSideVector = glm::cross(orientation, vec3(0.0f, 1.0f, 0.0f));
            glm::normalize(cameraSideVector);
        }


        // ZOOMING: NOT DONE!, Still needs work
        while (lastMouseLeftState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            orientation = vec3(0, sinf(phi), 0);
            vec3 cameraSideVector = glm::cross(orientation, vec3(0.0f, 1.0f, 0.0f));
            glm::normalize(cameraSideVector);
        }
    }

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void executeEvents(GLFWwindow *window, Camera &camera, float dt) {
    /* This function is used to execute all the inputs of the program
    * This program should only be called AFTER glfwPollEvents() was already called
    * 
    *   window - current window of the program
    *   camera - camera class of the scene (needed so we can execute the camera inputs)
    *   dt - Frame time
    */

    //Note: these have to be static so that their state does not get reset on each function call
    static bool ULastStateReleased = true, JLastStateReleased = true, PeriodLastStateReleased = true;

    camera.processInputs(window, dt); // processes all camera inputs

    // size up function. We monitor the last state of the button so that the resizing is more controlable
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE)
        ULastStateReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && ULastStateReleased) {
        scale *= (float) 8/7; // exponential resizing
        ULastStateReleased = false;
    }

    // size down function. We monitor the last state of the button so that the resizing is more controlable
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE)
        JLastStateReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && JLastStateReleased) {
        scale *= (float) 7/8; // exponential resizing; used so that negative sizes will not be created
        JLastStateReleased = false;
    }


    // change the draw mode
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        drawMode = GL_TRIANGLES;
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        drawMode = GL_LINES;
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        drawMode = GL_POINTS;



    // shuffle Jack's shape function
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_RELEASE)
        PeriodLastStateReleased = true;
    else if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS && PeriodLastStateReleased) {
        PeriodLastStateReleased = false;
        if (JacksShape == "Jack's Shape.csv")
            JacksShape = "Jack's Shape - Shuffle 1.csv";
        else if (JacksShape == "Jack's Shape - Shuffle 1.csv")
            JacksShape = "Jack's Shape - Shuffle 2.csv";
        else
            JacksShape = "Jack's Shape.csv";
    }
        
}

void renderShapeFromCSV(string filePath, glm::vec3 pos, GLfloat scale, GLenum drawMode, GLuint shaderProgram) {
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

            //now we draw the cube
            glm::mat4 cubeWorldMatrix = glm::translate(glm::mat4(1.0), glm::vec3(pos.x + cubeInfo[0], pos.y + cubeInfo[1], pos.z + cubeInfo[2])) * glm::scale(glm::mat4(1.0), glm::vec3(cubeInfo[3], cubeInfo[4], cubeInfo[5]));
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
