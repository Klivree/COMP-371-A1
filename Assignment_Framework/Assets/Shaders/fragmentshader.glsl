#version 330 core

out vec4 FragColor;

const float PI = 3.1415926535897932384626433832795;

struct Material {
    vec3 color;
    float shininess;
};

uniform Material material;

struct PointLight {
    vec3 POS;
    vec3 lightColor;
    float lightConstTerm;
    float lightLinearTerm;
    float lightQuadTerm;
    float lightFarPlane;
};

struct DirectionalLight {
    vec3 lightColor;
    vec3 lightDirection;
    mat4 lightSpaceMatrix;
};

struct SpotLight {
    vec3 POS;
    vec3 lightDirection;
    vec3 lightColor;
    float cutOffInner;
    float cutOffOuter;
};

uniform PointLight pointlight1;
uniform SpotLight spotlight1;
uniform SpotLight spotlight2;

// used so we can repeat the textures as needed
uniform float texWrapX = 1.0f;
uniform float texWrapY = 1.0f;

in vec2 textureCoords;
in vec3 vertexColor;
in vec3 fragmentNormal; // we need the normal in world space and not view space for lighting calcs
in vec3 fragmentPosition; // need fragment world POS for lighting

// constants to affect lighting
const float shadingAmbientStrength    = 0.2;
const float shadingDiffuseStrength    = 0.6;
const float shadingSpecularStrength   = 1.0;

uniform sampler2D modelTexture; // texture of the model being drawn
uniform samplerCube shadowMap; // the shadow depth cube map

uniform vec3 viewPosition;

uniform bool enableShadows;
uniform bool enableTextures;
uniform bool fullLight = false;


float cubeShadowScalar(PointLight pPointLight, samplerCube cubeShadowMap) {
	//returns 0.0 if the surface should recieve light, and 1.0 when it is in shadow

    vec3 fragmentToLight = fragmentPosition - pPointLight.POS;
    float closestDepth = texture(cubeShadowMap, fragmentToLight).r;
    // get into the proper range
    closestDepth *= pPointLight.lightFarPlane;
	//get depth of current fragment from the light
	float currentDepth = length(fragmentToLight);

	// bias is used to remove shadow acne, we do the max so we can accomidate bias for high and low angles
	float bias = max(0.05 * (1.0 - dot(fragmentNormal, fragmentToLight)), 0.005); 
        
	return ((currentDepth - bias) > closestDepth) ? 1.0:0.0;  // check if current frag is in shadow
}


vec3 calculateDirectionalLight(DirectionalLight dirLight) {
     vec3 lightDir = normalize(-dirLight.lightDirection);
     // diffused light
     float diffuseCoefficient = max(dot(lightDir, fragmentNormal), 0.0f);
     vec3 diffuse = shadingDiffuseStrength * diffuseCoefficient * dirLight.lightColor;

     // specular light
     vec3 viewDir = normalize(viewPosition - fragmentPosition);
     vec3 reflectDir = reflect(-lightDir, fragmentNormal);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0);
     vec3 specular = shadingSpecularStrength * spec * dirLight.lightColor;
     
     return (diffuse + specular);
}

vec3 calculateSpotLight(SpotLight pSpotLight) {
    pSpotLight.lightDirection = normalize(pSpotLight.lightDirection);

    float theta = acos(dot(normalize(fragmentPosition - pSpotLight.POS), pSpotLight.lightDirection)); // angle of fragment to the spotlight direction
    
    float lightIntensity;

    if(theta > pSpotLight.cutOffOuter) { // if outside of the spotlight range it gets no light
        lightIntensity = 0.0;
    } else if(theta > pSpotLight.cutOffInner) { // if between the outside and inside then gets some light
        lightIntensity = (1 - cos(PI * (theta - pSpotLight.cutOffOuter) / (pSpotLight.cutOffInner - pSpotLight.cutOffOuter))) / 2.0;
    } else { // receives full spotlight intensity when inside inner cutoff
        lightIntensity = 1.0f;
    }

    // diffuse light calculations
    vec3 lightDirection = normalize(pSpotLight.POS - fragmentPosition);
    float diffuseCoefficient = max(dot(fragmentNormal, lightDirection), 0.0f);
    vec3 diffuse = shadingDiffuseStrength * pSpotLight.lightColor * diffuseCoefficient;

    // specular light calculation
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 reflectDir = reflect(-lightDirection, fragmentNormal);
    float spec =  pow(max(dot(viewDirection, reflectDir), 0.0f), material.shininess * 128.0);
    vec3 specular = shadingSpecularStrength * pSpotLight.lightColor * spec;

    return lightIntensity * (diffuse + specular);
}

vec3 calculatePointLight(PointLight pPointLight) {
    vec3 lightDir = normalize(pPointLight.POS - fragmentPosition);

    // diffused light
    vec3 diffuse = max(dot(lightDir, fragmentNormal), 0.0f) * pPointLight.lightColor;

    // specular light
    vec3 viewDir = normalize(viewPosition - fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, fragmentNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0);
    vec3 specular = shadingSpecularStrength * spec * pPointLight.lightColor;

    //point light attenuation modifications so that the light will fade with distance
    float distanceFromLight = length(pPointLight.POS - fragmentPosition);
    float attenuation = 1.0 / (pPointLight.lightConstTerm + pPointLight.lightLinearTerm * distanceFromLight + pPointLight.lightQuadTerm * (distanceFromLight * distanceFromLight)); 
    diffuse *= attenuation;
    specular *= attenuation;

    return (diffuse + specular);
}


void main() {
    // get fragment color from the texture and times it by the vertex color
    vec3 color;
    if(enableTextures)
        color = texture(modelTexture, vec2(textureCoords.x * texWrapX - float(1*(texWrapX-1)/2), textureCoords.y * texWrapY - float((texWrapY-1)/2))).rgb * material.color;
    else 
        color =  material.color;

    // ambient light taken from the color from object textures
    vec3 ambient = shadingAmbientStrength * color; 

    float shadow;
    
    if(enableShadows)
         shadow = cubeShadowScalar(pointlight1, shadowMap); // getting shadow value
    else
         shadow = 0.0f;
    
    vec3 lightComponents = calculatePointLight(pointlight1) + calculateSpotLight(spotlight1) + calculateSpotLight(spotlight2);

    vec3 lighting = (ambient + (1.0f - shadow) * (lightComponents)) * color; // getting the overall lighting of the fragment
    if(fullLight) // toggle for models we want to be indpendent of light sources
        lighting = color;
    

    FragColor = vec4(lighting, 1.0f);
}
