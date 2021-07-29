#version 330 core

in vec4 FragPos;

uniform vec3 lightPosition;
uniform float lightFarPlane;

void main() {
	//distance between fragment and the light source
	float lightDistance = length(FragPos.xyz - lightPosition);

	// we get to [0,1] range
	lightDistance /= lightFarPlane;

	gl_FragDepth = lightDistance;

}