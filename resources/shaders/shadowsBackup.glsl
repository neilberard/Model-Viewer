#shader vertex
#version 330 core
// Simple Shader for rendering depth.

layout(location = 0) in vec4 position; // 1.0 is auto added when converting to vec4 from 3
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

layout (std140) uniform uBlock
{
    mat4 uProjection;
    mat4 uView;
};

uniform mat4 lightSpaceMatrix;
uniform mat4 uModel;

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
	
	vs_out.FragPos = vec3(uModel * position);
	vs_out.Normal = vec3(uModel * normal);
	vs_out.TexCoords = texCoord;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	
	
	// Tangents
	vec3 T = normalize(vec3(u_Model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(u_Model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(u_Model * vec4(normal, 0.0)));
	mat3 TBN = mat3(T, B, N);
	
	vs_out.Tangent = tangent;
	vs_out.TBN = TBN;
	
	gl_Position = uProjection * uView * vec4(vs_out.FragPos, 1.0);
}

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


uniform sampler2D u_ShadowMap;
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_NormalMap;
uniform sampler2D u_CheckerTexture;
uniform samplerCube u_Sky;

uniform vec3 u_Color;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float u_ShadowBias;

uniform int u_DrawTexture;
uniform int u_DrawReflection;
uniform int u_DrawShadow;
uniform int u_DrawMode;




float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	
	
	// PCF Soft shadows 
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
	
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <=1; ++y)
		{
			float pcfDepfth = texture(u_ShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
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
		
	
	vec3 color = u_Color;
	
	if(u_DrawTexture == 1)
	{
		color = (texture(u_DiffuseTexture, fs_in.TexCoords).rgb + vec3(0.1)) / 2.0;
	}
	
	
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightColor = vec3(1.0);
	vec3 ambient = 0.15 * color;
	
	// Diffuse
	//vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 lightDir = normalize(lightPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

			 
	 // Reflection
	vec3 I = normalize(fs_in.FragPos - viewPos);
	vec3 R = reflect(I, normal);
	
	
	vec3 reflectColor = vec3(texture(u_Sky, R).rgb) * .8;
	if(u_DrawReflection != 1)
	{
		reflectColor = vec3(0.0);
	}


	// Spec
	vec3 veiwDir = normalize(viewPos - fs_in.FragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + veiwDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;
	
	
	float bias = max(u_ShadowBias * (1.0 - dot(normal, lightDir)), 0.02);
	
	float shadow = 0.0f;
	if(u_DrawShadow == 1)
	{
	shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);
	}
	
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular + reflectColor)) * color;
	FragColor = vec4(lighting, 1.0);
}

