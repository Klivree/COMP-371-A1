#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

float scale = 0.15f;

char* readFile(const char* filePath) { //credit: https://badvertex.com/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c.html
    string content;
    ifstream fileStream(filePath, ios::in);

    if (!fileStream.is_open()) {
        cerr << "Could not read file " << filePath << ". File does not exist." << endl;
        return new char[0];
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

GLuint compileAndLinkShaders() {
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // create vertex shader
    char* vertexShaderSource = getVertexShaderSource();
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
    char* fragmentShaderSource = getFragmentShaderSource();
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

GLuint getCubeModel(glm::vec3 color) {
    // Cube model
    glm::vec3 vertexArray[] = {  
        // position,                  color
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

void renderShapeFromCSV(string filePath, glm::vec3 pos, GLfloat scale, GLuint shaderProgram) {
    /** Renders the shape specified in the CSV file provided by using transformed cube models. A cube model should be binded before this method is called.
    * filePath - filePath of the .csv file that defines the shape
    * pos - coordinates of the object
    * scale - how much the object is to be scaled by 
    * shaderProgram - shader program used by the running OpenGL application
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
            else
                value.append(string(1, curChar)); // add the char to the current value

            // line is finished
            if (curChar == '\n') 
                break;
        }

        if (j != 6) { //if we didnt get to 6 then we either have too many or too little values per line
            cerr << "There should be exclusively 6 values per line of the csv" << endl;
            return;
        }
       
        //scale the values
        for (int k = 0; k < 6; k++) {
            cubeInfo[k] = scale * cubeInfo[k];
        }

        //now we draw the cube
        glm::mat4 cubeWorldMatrix = glm::translate(glm::mat4(1.0), glm::vec3(pos.x + cubeInfo[0], pos.y + cubeInfo[1], pos.z + cubeInfo[2])) * glm::scale(glm::mat4(1.0), glm::vec3(cubeInfo[3], cubeInfo[4], cubeInfo[5]));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &cubeWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Comp371 - Assignment 1", NULL, NULL);
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

    //get Shape cubes
    GLuint shapeVAO = getCubeModel(glm::vec3(1.0f, 1.0f, 0.0f));

    GLuint wallVAO = getCubeModel(glm::vec3(0.5f, 0.5f, 0.5f));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT); //clear the window each frame
        glUseProgram(shaderProgram);

        //draw the shapes
        glBindVertexArray(shapeVAO);
        renderShapeFromCSV("shapefilepath.csv", glm::vec3(0.0,0.0,0.0), 0.2, shaderProgram);
        glBindVertexArray(0);


        //draw the wall models
        glBindVertexArray(wallVAO);
        renderShapeFromCSV("shapeHoleFilepath.csv", glm::vec3(0.0,0.0,2.0), 0.2, shaderProgram);
        glBindVertexArray(0);


        glfwSwapBuffers(window); //swap the front buffer with back buffer

        glfwPollEvents();
    }

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}