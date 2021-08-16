#ifndef TEXT_RENDER_HEADER
#define TEXT_RENDER_HEADER

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glew-2.1.0/include/GL/glew.h>

using namespace std;
using namespace glm;

GLuint pixelVAO;
bool initialized = false;
using namespace std;
using namespace glm;

// Letter Pixel Art from https://www.123rf.com/photo_34046590_stock-vector-alphabet-and-numbers-pixel-art-style.html

bool A[] {
	false, true, false,
	true, false, true,
	true, true, true,
	true, false, true,
	true, false, true
};

bool B[] {
	true, true, true,
	true, false, true,
	true, true, false,
	true, false, true,
	true, true, true
};

bool C[] {
	true, true, true,
	true, false, false,
	true, false, false,
	true, false, false,
	true, true, true
};

bool D[] {
	true, true, false,
	true, false, true,
	true, false, true,
	true, false, true,
	true, true, false
};

bool E[] {
	true, true, true,
	true, false, false,
	true, true, true,
	true, false, false,
	true, true, true
};

bool F[] {
	true, true, true,
	true, false, false,
	true, true, false,
	true, false, false,
	true, false, false
};

bool G[] {
	true, true, true,
	true, false, false,
	true, false, false,
	true, false, true,
	true, true, true
};

bool H[] {
	true, false, true,
	true, false, true,
	true, true, true,
	true, false, true,
	true, false, true
};

bool I[] {
	true, true, true,
	false, true, false,
	false, true, false,
	false, true, false,
	true, true, true
};

bool J[] {
	false, false, true,
	false, false, true,
	false, false, true,
	true, false, true,
	true, true, true
};

bool K[] {
	true, false, true,
	true, false, true,
	true, true, false,
	true, false, true,
	true, false, true
};

bool L[] {
	true, false, false,
	true, false, false,
	true, false, false,
	true, false, false,
	true, true, true
};

bool M[] {
	true, false, false, false, true,
	true, true, false, true, true,
	true, false, true, false, true,
	true, false, false, false, true,
	true, false, false, false, true
};

bool N[] {
	true, false, false, true,
	true, false, false, true,
	true, true, false, true,
	true, false, true, true,
	true, false, false, true
};

bool O[] {
	true, true, true,
	true, false, true,
	true, false, true,
	true, false, true,
	true, true, true
};

bool P[] {
	true, true, true,
	true, false, true,
	true, true, true,
	true, false, false,
	true, false, false
};

bool Q[] {
	true, true, true,
	true, false, true,
	true, false, true,
	true, true, false,
	false, false, true
};

bool R[] {
	true, true, true,
	true, false, true,
	true, true, false,
	true, false, true,
	true, false, true
};

bool S[] {
	true, true, true,
	true, false, false ,
	false, true, false,
	false, false, true,
	true, true, true
};

bool T[] {
	true, true, true,
	false, true, false,
	false, true, false,
	false, true, false,
	false, true, false
};

bool U[] {
	true, false, true,
	true, false, true,
	true, false, true,
	true, false, true,
	true, true, true
};

bool V[] {
	true, false, true,
	true, false, true,
	true, false, true,
	true, false, true,
	false, true, false
};

bool W[] {
	true, false, false, false, true,
	true, false, false, false, true,
	true, false, true, false, true,
	true, false, true, false, true,
	false, true, false, true, false
};

bool X[] {
	true, false, true,
	true, false, true,
	false, true, false,
	true, false, true,
	true, false, true
};

bool Y[] {
	true, false, true,
	true, false, true,
	true, false, true,
	false, true, false,
	false, true, false
};

bool Z[]{
	true, true, true,
	false, false, true,
	false, true, false,
	true, false, false,
	true, true, true
};

bool one[] {
	true,
	true,
	true,
	true,
	true
};

bool two[] {
	true, true, true,
	false, false, true,
	false, true, false,
	true, false, false,
	true, true, true
};

bool three[] {
	true, true, true,
	false, false, true,
	false, true, false,
	false, false, true,
	true, true, true
};

bool four[] {
	true, false, true,
	true, false, true,
	true, true, true,
	false, false, true,
	false, false, true
};

bool five[] {
	true, true, true,
	true, false, false,
	true, true, true,
	false, false, true,
	true, true, true
};

bool six[] {
	true, true, true,
	true, false, false,
	true, true, true,
	true, false, true,
	true, true, true
};

bool seven[] {
	true, true, true,
	false, false, true,
	false, false, true,
	false, false, true,
	false, false, true
};

bool eight[] {
	true, true, true,
	true, false, true,
	true, true, true,
	true, false, true,
	true, true, true
};

bool nine[] {
	true, true, true,
	true, false, true,
	true, true, true,
	false, false, true,
	true, true, true
};

bool zero[] {
	true, true, true,
	true, false, true,
	true, false, true,
	true, false, true,
	true, true, true
};

bool space[]{
	false, false, false,
	false, false, false,
	false, false, false,
	false, false, false,
	false, false, false
};

bool plus[] {
	false, false, false,
	false, true, false,
	true, true, true,
	false, true, false,
	false, false, false,
};

bool minus[] {
	false, false, false,
	false, false, false,
	true, true, true,
	false, false, false,
	false, false, false,
};

bool equal[]{
	false, false, false,
	true, true, true,
	false, false, false,
	true, true, true,
	false, false, false
};

bool questionMark[]{
	true, true, true,
	false, false, true,
	false, true, false,
	false, false, false,
	false, true, false
};

bool exclamation[] {
	true, true, true,
	true, true, true,
	false, true, false,
	false, false, false,
	false, true, false
};

bool period[]{
	false,
	false,
	false,
	false,
	true
};

bool colon[] {
	false,
	true,
	false,
	true,
	false
};

bool nullChar[] {
	true, true, true, false, true, true, true,
	true, false, true, false, true, false, true,
	true, false, false, true, false, false, true,
	true, false, false, false, false, false, true,
	false, true, false, false, false, true, false,
	false, false, true, false, true, false, false,
	false, false, false, true, false, false, false
};

void init() {
	vec3 pixel[]{
		vec3(-.5,.5,0),
		vec3(-.5,-.5,0),
		vec3(.5,.5,0),
		

		vec3(.5,-.5,0),
		vec3(.5,.5,0),
		vec3(-.5,-.5,0)
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pixel), pixel, GL_STATIC_DRAW);

	//create position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	pixelVAO = VAO; // save the VAO
	initialized = true; // flag the renderer
}

int drawLetter(char letter, vec3 pos, vec3 color, GLfloat pScale, GLuint shaderProgram) {
	if (!initialized)
		::init();

	bool* charInfo;
	int width = 3;
	int height = 5;

	switch (letter) {
	case 'A':
		charInfo = A;
		break;
	case 'B':
		charInfo = B;
		break;
	case 'C':
		charInfo = C;
		break;
	case 'D':
		charInfo = D;
		break;
	case 'E':
		charInfo = E;
		break;
	case 'F':
		charInfo = F;
		break;
	case 'G':
		charInfo = G;
		break;
	case 'H':
		charInfo = H;
		break;
	case 'I':
		charInfo = I;
		break;
	case 'J':
		charInfo = J;
		break;
	case 'K':
		charInfo = K;
		break;
	case 'L':
		charInfo = L;
		break;
	case 'M':
		charInfo = M;
		width = 5;
		break;
	case 'N':
		charInfo = N;
		width = 4;
		break;
	case 'O':
		charInfo = O;
		break;
	case 'P':
		charInfo = P;
		break;
	case 'Q':
		charInfo = Q;
		break;
	case 'R':
		charInfo = R;
		break;
	case 'S':
		charInfo = S;
		break;
	case 'T':
		charInfo = T;
		break;
	case 'U':
		charInfo = U;
		break;
	case 'V':
		charInfo = V;
		break;
	case 'W':
		charInfo = W;
		width = 5;
		break;
	case 'X':
		charInfo = X;
		break;
	case 'Y':
		charInfo = Y;
		break;
	case 'Z':
		charInfo = Z;
		break;
	case '1':
		charInfo = ::one;
		width = 1;
		break;
	case '2':
		charInfo = ::two;
		break;
	case '3':
		charInfo = ::three;
		break;
	case '4':
		charInfo = ::four;
		break;
	case '5':
		charInfo = ::five;
		break;
	case '6':
		charInfo = ::six;
		break;
	case '7':
		charInfo = ::seven;
		break;
	case '8':
		charInfo = ::eight;
		break;
	case '9':
		charInfo = ::nine;
		break;
	case '0':
		charInfo = ::zero;
		break;
	case '-':
		charInfo = ::minus;
		break;
	case '+':
		charInfo = ::plus;
		break;
	case '=':
		charInfo = ::equal;
		break;
	case ' ':
		charInfo = space;
		break;
	case '?':
		charInfo = questionMark;
		break;
	case '!':
		charInfo = exclamation;
		break;
	case '.':
		charInfo = ::period;
		width = 1;
		break;
	case ':':
		charInfo = ::colon;
		width = 1;
		break;
	default:
		charInfo = nullChar;
		width = 7;
		height = 7;
		break;
	}

	glUseProgram(shaderProgram);
	glBindVertexArray(pixelVAO);

	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

	mat4 worldMatrix(1.0);
	worldMatrix = translate(worldMatrix, pos);
	worldMatrix = scale(worldMatrix, vec3(pScale));

	//set color of the letter
	glUniform3fv(glGetUniformLocation(shaderProgram, "aColor"), 1, &color[0]);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			if(charInfo[width*i + j])
				glDrawArrays(GL_TRIANGLES, 0, 6);

			worldMatrix = translate(worldMatrix, vec3(1.0f, 0.0f, 0.0f));
		}
		worldMatrix = translate(worldMatrix, vec3(-width, 0.0f, 0.0f)); // reset back to left of
		worldMatrix = translate(worldMatrix, vec3(0.0f, -1.0f, 0.0f)); // move one row down
	}

	glBindVertexArray(0);

	return width;
}

void drawString(string stringToDraw, vec3 pos, vec3 color, GLfloat pScale, GLuint shaderProgram) {
	char charToDraw;
	int drawnWidth;
	vec3 drawPOS = pos;
	vec3 startOfLine = drawPOS;

	for (int i = 0; i < stringToDraw.size(); i++) {
		charToDraw = toupper(stringToDraw[i]);
		if (charToDraw == '\n') { // create a new line
			startOfLine += vec3(0.0f, -7.0f, 0.0f) * pScale;
			drawPOS = startOfLine;
		}
		else { // draw a letter
			drawnWidth = drawLetter(charToDraw, drawPOS, color, pScale, shaderProgram);
			drawPOS += vec3((drawnWidth + 1) * pScale, 0, 0);
		}
	}
}

class stringFlickeringEngine {
public:
	stringFlickeringEngine(vec3 pBaseColor, vec3 pFlickerColor, GLfloat pTimeBetweenFlickers, GLuint pNumofFlickers) {
		baseColor = pBaseColor;
		flickerColor = pFlickerColor;
		numOfFlickers = pNumofFlickers;
		timeBetweenFlickers = pTimeBetweenFlickers;
		lastFlickerTime = glfwGetTime();
	}

	void drawText(string pStringToDraw, vec3 pPOS, GLfloat pScale, GLuint shaderProgram) { drawText(false, pStringToDraw, pPOS, pScale, shaderProgram); }

	void drawText(bool resetFlicker, string pStringToDraw, vec3 pPOS, GLfloat pScale, GLuint shaderProgram) {
		vec3 drawColor;
		GLfloat newTime = glfwGetTime();

		if (resetFlicker)
			currentlyFlickering = true;

		if (currentlyFlickering) {
			if (curFlickerNum >= numOfFlickers) { // finish flickering
				currentlyFlickering = false;
				drawColor = baseColor;
				curFlickerNum = 0;
			}
			else {
				if (lastFlickerOn)
					drawColor = baseColor;
				else
					drawColor = flickerColor;

				if (newTime - lastFlickerTime > timeBetweenFlickers) {
					lastFlickerOn = !lastFlickerOn;
					lastFlickerTime = newTime;
					curFlickerNum++;
				}
			}
		}
		else
			drawColor = baseColor;

		drawString(pStringToDraw, pPOS, drawColor, pScale, shaderProgram);
	}

private:
	vec3 baseColor;
	vec3 flickerColor;

	GLfloat lastFlickerTime;
	GLfloat timeBetweenFlickers;
	GLuint numOfFlickers;

	int curFlickerNum = 0;

	bool lastFlickerOn = false;
	bool currentlyFlickering = false;

};

#endif