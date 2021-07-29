#version 330 core

out vec4 FragColor;

in vec2 textureCoords;
in vec3 vertexColor;
in vec3 ambientLightColor;
in vec3 fragmentNormal;
in vec3 fragmentPosition;
in vec4 fragmentPositionLightSpace;

// constants to affect lighting
const float shadingAmbientStrength    = 0.1;
const float shadingDiffuseStrength    = 0.6;
const float shadingSpecularStrength   = 0.3;

uniform sampler2D modelTexture;
uniform samplerCube shadowMap;

uniform vec3 viewPosition;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform float lightConstTerm;
uniform float lightLinearTerm;
uniform float lightQuadTerm;
uniform float lightFarPlane;

/*float shadow_scalar() {
	//returns 1.0 if the surface should recieve light, and 0.0 when it is in shadow

	//perspective divide
	vec3 projectedCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
	//transform to [0,1] range
	projectedCoords = projectedCoords * 0.5 + 0.5;
	//get closest depth value
	float closestDepth = texture(shadowMap, projectedCoords.xy).r;
	//get depth of current fragment from the light
	float currentDepth = projectedCoords.z;
	//check if current frag is in shadow
	float bias = 0.005; // bias is used to remove shadow acne
	return ((currentDepth - bias) > closestDepth) ? 1.0:0.0;
}*/



//THIS IS PROBABLT MESSED UP STILL
float cubeShadowScalar() {
	//returns 1.0 if the surface should recieve light, and 0.0 when it is in shadow

    vec3 fragmentToLight = fragmentPosition - lightPosition;
    float closestDepth = texture(shadowMap, fragmentToLight).r;
    // get into the proper range
    closestDepth *= lightFarPlane;
	//get depth of current fragment from the light
	float currentDepth = length(fragmentToLight);
	//check if current frag is in shadow
	float bias = 0.005; // bias is used to remove shadow acne
	return ((currentDepth - bias) > closestDepth) ? 1.0:0.0;
}


void main()
{
    
    vec3 lightDir = normalize(lightPosition - fragmentPosition);

    // get fragment color from the texture and times it by the vertex color
    vec3 color = texture(modelTexture, textureCoords).rgb * vertexColor;

    // ambient light taken from the color from object textures
    vec3 ambient = shadingAmbientStrength * color; 
    
    // diffused light
    vec3 diffuse = max(dot(lightDir, fragmentNormal), 0.0f) * lightColor;

    // specular light
    vec3 viewDir = normalize(viewPosition - fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, fragmentNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    vec3 specular = shadingSpecularStrength * spec * lightColor;

    float shadow = cubeShadowScalar(); // getting shadow value

    //point light attenuation modifications so that the light will fade with distance
    float distanceFromLight = length(lightPosition - fragmentPosition);
    float attenuation = 1.0 / (lightConstTerm + lightLinearTerm * distanceFromLight + lightQuadTerm * (distanceFromLight * distanceFromLight)); 
    diffuse *= attenuation;
    specular *= attenuation;


    vec3 lighting = (ambient + (1.0f - 0.0) * (diffuse + specular)) * color; // NOTE: SHADOW IS PRETTY MESSED UP RN, REPLACE shadow WITH 0.0 TO SEE LIGHTING IN ACTION   
    

    FragColor = vec4(lighting, 1.0f);
}