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
	mat4 uModel;
};

uniform mat4 lightSpaceMatrix;


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



	// Shadow Info
	vs_out.FragPos = vec3(uModel * position);
	vs_out.Normal = vec3(uModel * normal);
	vs_out.TexCoords = texCoord;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);


	// Tangents
	vec3 T = normalize(vec3(uModel * vec4(tangent, 0.0)));
	//vec3 B = normalize(vec3(uModel * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(uModel * normal));
	vec3 B = cross(N,T);

	T = normalize(T - dot(T, N) * N);
	
	vs_out.Tangent = tangent;
	//vs_out.TBN = transpose(mat3(T,B,N));
	vs_out.TBN = mat3(T,B,N);
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
uniform sampler2D uDiffuseMap;
uniform sampler2D uNormalMap;
uniform samplerCube uSky;

uniform vec3 uColor;
uniform vec3 lightPos;
uniform vec3 uViewPos;
uniform float uShadowBias;
uniform float uSpecular;
uniform float uGlossiness;


uniform int uWireframe;
uniform int uDrawTexture;
uniform int uDrawNormal;
uniform int uDrawReflection;
uniform int uDrawShadow;
uniform int uDrawMode; // Debugging

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

	FragColor = vec4(uColor, 1.0);
	vec3 normal = normalize(fs_in.Normal);
	vec3 color = uColor;

	if(uWireframe == 1)
	{
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);	
		return;		
	}
	
	
	if(uDrawTexture == 1)
	{
		color = texture(uDiffuseMap, fs_in.TexCoords).rgb;
	}
	

	if(uDrawNormal == 1)
	{
	normal = texture(uNormalMap, fs_in.TexCoords).rgb;
	normal = (normal * 2.0) - 1.0;
	normal = normalize(fs_in.TBN * normal);
	//normal.y = -normal.y;
	}
	
	vec3 lightColor = vec3(1.0);
	vec3 ambient = 0.15 * uColor;
	
	
	// Diffuse
	//vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 veiwDir = normalize(uViewPos - fs_in.FragPos);
	
	
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;
	 
	 
	// Reflection
	vec3 I = normalize(fs_in.FragPos - uViewPos);
	vec3 R = reflect(I, normal);
	
	vec3 reflectColor = vec3(texture(uSky, R).rgb) * .8;
	if(uDrawReflection != 1)
	{
		reflectColor = vec3(0.0);
	}


	// Spec

	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + veiwDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), uGlossiness);
	vec3 specular = spec * lightColor * uSpecular;
	
	
	float bias = max(uShadowBias * (1.0 - dot(normal, lightDir)), 0.02);
	
	float shadow = 0.0f;
	if(uDrawShadow == 1)
	{
	shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);
	}
	
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular + reflectColor)) * color;
	//FragColor = vec4(lighting, 1.0);
	
	
	if(uDrawMode == 1)
	{
		FragColor = vec4(normal, 1.0);
		return;

	}
	
	
	FragColor = vec4(lighting, 1.0);
	
	
	
	
}

