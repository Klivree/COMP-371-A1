#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 vertexColor;
void main()
{
	vertexColor = vec3(1.0f, 1.0f, 1.0f);
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
