#shader vertex
#version 330 core
// Simple Shader for rendering depth.

layout(location = 0) in vec3 position; // 1.0 is auto added when converting to vec4 from 3
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;


out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} vs_out;

uniform mat4 lightSpaceMatrix;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;



void main()
{
	
	vs_out.FragPos = vec3(u_Model * vec4(position, 1.0));
	vs_out.Normal = transpose((mat3(u_Model))) * normal;
	vs_out.TexCoords = texCoord;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	gl_Position = u_Projection * u_View * vec4(vs_out.FragPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;


uniform sampler2D u_ShadowMap;
uniform sampler2D u_Texture;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int drawTexture;


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
	vec3 color = texture(u_Texture, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightColor = vec3(1.0);
	vec3 ambient = 0.15 * color;
	
	// Diffuse
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	// Spec
	vec3 veiwDir = normalize(viewPos - fs_in.FragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + veiwDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;
	
	
	float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	FragColor = vec4(lighting, 1.0);
}

