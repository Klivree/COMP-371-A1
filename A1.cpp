#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

float scale = 0.15f;

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

GLuint getObject() {
    GLfloat vertices[] = {
        0.0f * scale, 0.0f * scale, 0.0f * scale, //0: lower left of L long
        0.0f * scale, 5.0f * scale, 0.0f * scale, //1: upper left of L long
        1.0f * scale, 0.0f * scale, 0.0f * scale, //2: lower right of L long
        1.0f * scale, 5.0f * scale, 0.0f * scale, //3: upper right of L long
        1.0f * scale, 1.0f * scale, 0.0f * scale, //4: upper left of L leg
        3.0f * scale, 1.0f * scale, 0.0f * scale, //5: upper right of L leg
        3.0f * scale, 0.0f * scale, 0.0f * scale, //6: lower right of L leg
    };

    GLuint indices[] = {
        0,1,2,
        1,3,2,
        4,5,2,
        2,6,5
    };


    //create VAO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //get position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind the VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //unbind EBO after VAO so VAO knows it should keep using the EBO

    return VAO;
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

    //get VAO
    GLuint VAO = getObject();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT); //clear the window each frame
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);




        glfwSwapBuffers(window); //swap the front buffer with back buffer

        glfwPollEvents();
    }

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}