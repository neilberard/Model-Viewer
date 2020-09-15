#shader vertex
#version 330 core

layout(location = 0) in vec4 position; 
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

layout (std140) uniform uBlock
{
    mat4 uProjection;
    mat4 uView;
	int uSky;
	int uShadows;
};

uniform mat4 lightSpaceMatrix;
uniform mat4 uModel;

// BOOL
uniform int uWireframe;



out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
} vs_out;



void main()
{	
vec4 zOffset = vec4(0.0, 0.0, 0.0, 0.0);

if(uWireframe != 0)
{
	zOffset = vec4(0.0, 0.0, 0.0005, 0.0);
}
	gl_Position = (uProjection * uView * uModel * position) - zOffset;
}


// ------------------------------------------------------------------------- //

#shader fragment
#version 330 core

out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
	
} fs_in;


uniform sampler2D uShadowMap;
uniform sampler2D uDiffuseTexture;
uniform sampler2D uNormalMap;
uniform sampler2D uCheckerTexture;
uniform samplerCube uSky;

uniform vec3 uColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float uShadowBias;

uniform int uWireframe;
uniform int uDrawTexture;
uniform int uDrawReflection;
uniform int uDrawShadow;
uniform int uDrawMode;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(uShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	
	
	// PCF Soft shadows 
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
	
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <=1; ++y)
		{
			float pcfDepfth = texture(uShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepfth ? 1.0 : 0.0;
		}
	}
	
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0
	shadow /= 9.0;
	
	if(projCoords.z > 1.0) shadow = 0.0;
	return shadow;
}


void main()
{	

	FragColor = vec4(1.0, 0.0, 0.0, 1.0);

	if(uWireframe == 1)
	{
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);		
	}

}


