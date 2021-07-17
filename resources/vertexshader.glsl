#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vertexColor;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix = mat4(1.0);

void main()
{
	vertexColor = aColor;
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(aPos, 1.0);
}
