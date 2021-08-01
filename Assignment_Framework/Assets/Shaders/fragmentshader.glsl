#version 330 core

out vec4 FragColor;

struct Material {
    vec3 ambient; //or color
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

in vec2 textureCoords;
in vec3 vertexColor;
in vec3 fragmentNormal; // we need the normal in world space and not view space for lighting calcs
in vec3 fragmentPosition; // need fragment world POS for lighting

// constants to affect lighting
const float shadingAmbientStrength    = 0.2;
const float shadingDiffuseStrength    = 0.5;
const float shadingSpecularStrength   = 1.0;

uniform sampler2D modelTexture; // texture of the model being drawn
uniform samplerCube shadowMap; // the shadow depth cube map

uniform vec3 viewPosition;

//light uniforms
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform float lightFarPlane; // used so that we can the closest depth term to the proper size again in the shadow scalar function
// values for the light attenuation function (controls how quickly the point light's range decreases
uniform float lightConstTerm;
uniform float lightLinearTerm;
uniform float lightQuadTerm;

uniform bool enableShadows;
uniform bool enableTextures;


float cubeShadowScalar() {
	//returns 0.0 if the surface should recieve light, and 1.0 when it is in shadow

    vec3 fragmentToLight = fragmentPosition - lightPosition;
    float closestDepth = texture(shadowMap, fragmentToLight).r;
    // get into the proper range
    closestDepth *= lightFarPlane;
	//get depth of current fragment from the light
	float currentDepth = length(fragmentToLight);

	float bias = 0.05; // bias is used to remove shadow acne
        
	return ((currentDepth - bias) > closestDepth) ? 1.0:0.0;  // check if current frag is in shadow
}


void main() {
    vec3 lightDir = normalize(lightPosition - fragmentPosition);

    // get fragment color from the texture and times it by the vertex color
    vec3 color;
    if(enableTextures)
        color = texture(modelTexture, textureCoords).rgb * material.ambient;
    else 
        color =  material.ambient;

    // ambient light taken from the color from object textures
    vec3 ambient = shadingAmbientStrength * color; 
    
    // diffused light
    vec3 diffuse = max(dot(lightDir, fragmentNormal), 0.0f) * lightColor * material.diffuse;

    // specular light
    vec3 viewDir = normalize(viewPosition - fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, fragmentNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0);
    vec3 specular = shadingSpecularStrength * spec * lightColor * material.specular;


    //point light attenuation modifications so that the light will fade with distance
    float distanceFromLight = length(lightPosition - fragmentPosition);
    float attenuation = 1.0 / (lightConstTerm + lightLinearTerm * distanceFromLight + lightQuadTerm * (distanceFromLight * distanceFromLight)); 
    diffuse *= attenuation;
    specular *= attenuation;

    float shadow;
    
    if(enableShadows)
         shadow = cubeShadowScalar(); // getting shadow value
    else
         shadow = 0.0f;

    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color; // getting the overall lighting of the fragment
    

    FragColor = vec4(lighting, 1.0f);
}