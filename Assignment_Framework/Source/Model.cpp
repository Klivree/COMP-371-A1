#include "Model.hpp"

Model::Model(std::string pFilePath, glm::vec3 pPOS, GLfloat pScale, GLenum pDrawMode) {
    filePath = pFilePath;

    initialPOS = pPOS;
    POS = initialPOS;

    initialScale = pScale;
    scale = initialScale;

    initialDrawMode = pDrawMode;
    drawMode = initialDrawMode;

    initialQuat = quat(vec3(0.0f));
    rotationQuat = initialQuat;

    material = Material(); // default white plastic material

    initializeModel();
}

Model::Model(std::string pFilePath, glm::vec3 pPOS, GLuint pTexture) {
    filePath = pFilePath;

    initialPOS = pPOS;
    POS = initialPOS;

    initialScale = 1.0f;
    scale = initialScale;

    initialDrawMode = GL_TRIANGLES;
    drawMode = initialDrawMode;

    initialQuat = quat(vec3(0.0f));
    rotationQuat = initialQuat;

    material = Material(); // default white plastic material

    texture = pTexture;

    initializeModel();
}

void Model::resetModel() {
    POS = initialPOS;
    rotationQuat = initialQuat;
    scale = initialScale;
    drawMode = initialDrawMode;
}

void Model::linkVAO(GLuint pVAO, int pActiveVertices) {
    VAO = pVAO;
    activeVertices = pActiveVertices;
}

void Model::render(GLuint shaderProgram, bool enableTextures) { render(shaderProgram, enableTextures, glm::mat4(1.0f)); }

void Model::render(GLuint shaderProgram, bool enableTextures, glm::mat4 baseMatrix) {
    //initializeModel(); // will make the model reread the csv file every draw - Uncomment if you want to make the objects in real time
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.color"), 1, &material.color[0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), material.shininess);

    glUniform1i(glGetUniformLocation(shaderProgram, "enableTextures"), enableTextures);
    


    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    baseMatrix = glm::translate(baseMatrix, POS); 
    baseMatrix = baseMatrix * toMat4(rotationQuat);

    // allow for texture wrapping
    glUniform1f(glGetUniformLocation(shaderProgram, "texWrapX"), texWrapX);
    glUniform1f(glGetUniformLocation(shaderProgram, "texWrapY"), texWrapY);

    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

    for (cubeInfo info : information) {
        glm::vec3 localCoord = scale * glm::vec3(info.posX, info.posY, info.posZ);
        glm::vec3 scalingVector = scale * glm::vec3(info.scaleX, info.scaleY, info.scaleZ);

        // transformation of the base matrix
        glm::mat4 cubeWorldMatrix = glm::translate(baseMatrix, localCoord);
        cubeWorldMatrix = glm::scale(cubeWorldMatrix, scalingVector);

        // draw the cube
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &cubeWorldMatrix[0][0]);

        // change the draw mode of the model being rendered
        if (drawMode == GL_TRIANGLES)
            glDrawArrays(GL_TRIANGLES, 0, activeVertices);
        else if (drawMode == GL_LINES) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // turn on wireframe
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // turn off wireframe
        }
        else if (drawMode == GL_POINTS)
            glDrawArrays(GL_POINTS, 0, 36);
        else
            std::cerr << "Invalid draw type. Renderer supports: GL_TRIANGLES, GL_LINES, GL_POINTS" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void Model::render(GLuint shaderProgram) { render(shaderProgram, true); }

void Model::linkTexture(GLuint pTexture) {
    texture = pTexture;
}

void Model::setMaterial(Material pMaterial) {
    material = pMaterial;
}

void Model::updateFilePath(std::string pFilePath) {
    filePath = pFilePath;
    initializeModel(); // have to reread the file
}

std::string Model::getFilePath() {
    return filePath;
}

void Model::initializeModel() {
    information.clear();
    if (filePath.empty()) { // add support for simple shapes
        information.push_back(cubeInfo(0, 0, 0, 1, 1, 1));
        return;
    }

    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return;
    }

    std::string value, line = "";
    float cubeInformation[6] = {};
    int i, j;
    char curChar;

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

            if (curChar == ',' || (curChar == '\n' && j >= 5)) {//need to push the info when these chars appear
                cubeInformation[j++] = stof(value); // push float value to the array
                value = "";
            }
            else if (curChar != ' ') // ignore spaces
                value.append(std::string(1, curChar)); // add the char to the current value

             // line is finished
            if (curChar == '\n' || curChar == '/' || j == 6)
                break;
        }

        if (!(j < 6))
            information.push_back(cubeInfo(cubeInformation[0], cubeInformation[1], cubeInformation[2], cubeInformation[3], cubeInformation[4], cubeInformation[5]));
    }
}