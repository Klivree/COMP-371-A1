#include "Model.hpp"

Model::Model(std::string pFilePath, GLuint pVAO, glm::vec3 pPosition, GLenum pDrawMode) {
	filepath = pFilePath;
	VAO = pVAO;
	initialPOS = pPosition;
	position = initialPOS;
	defaultDrawMode = pDrawMode;
	drawMode = pDrawMode;
	rotationVector = initialRotationVector;
}

void Model::render(GLuint shaderProgram) {

    glBindVertexArray(VAO);

    std::ifstream fileStream(filepath, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << filepath << ". File does not exist." << std::endl;
        return;
    }

    std::string value, line = "";
    float cubeInfo[6] = {};
    int i, j;
    char curChar;
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

    //creation of base matrix
    glm::mat4 baseMatrix = glm::mat4(1.0f);
    baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationVector.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate around x axis
    baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationVector.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate around y axis
    baseMatrix = glm::rotate(baseMatrix, glm::radians(rotationVector.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate around z axis
    baseMatrix = glm::translate(baseMatrix, position);

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
                value.append(std::string(1, curChar)); // add the char to the current value

             // line is finished
            if (curChar == '\n' || curChar == '/' || j == 6)
                break;
        }

        if (j < 6)  //if we didnt get to 6 then we either have too little values per line
            std::cout << "There are not enough values to fully define an object" << std::endl;
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
                std::cerr << "Invalid draw type. Reader supports: GL_TRIANGLES, GL_LINES, GL_POINTS" << std::endl;

        }
    }
    glBindVertexArray(0); // unbind the VAO
}

void Model::resetModel() {
    position = initialPOS;
    rotationVector = initialRotationVector;
    scale = initialScale;
}