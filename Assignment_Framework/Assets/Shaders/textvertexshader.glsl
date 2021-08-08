#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vertexColor;

uniform vec3 aColor;

uniform mat4 worldMatrix;

void main() {
	gl_Position = worldMatrix * vec4(aPos, 1.0f);

	vertexColor = aColor;
}

