#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;
//layout (location = 3) in vec3 aColor;


out vec3 vertexColor;
out vec3 fragmentNormal;
out vec3 fragmentPosition;
out vec4 fragmentPositionLightSpace;
out vec2 textureCoords;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix = mat4(1.0);

void main()
{
	// normalize to get unit vector, transpose the inverse of world matrix to add support for non uniform scalings
	fragmentNormal = normalize(mat3(transpose(inverse(worldMatrix))) * aNormals); 
	// fragment position should be in world space so we need to not multiply the proj or view matrices
    fragmentPosition = vec3(worldMatrix * vec4(aPos, 1.0)); 

	textureCoords = aTexCoords;

	//vertexColor = aColor;
	// the position on the screen of the vertices
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(aPos, 1.0);
}