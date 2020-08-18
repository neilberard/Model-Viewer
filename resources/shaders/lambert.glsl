#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // 1.0 is auto added when converting to vec4 from 3
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 barycentric;


out vec3 v_BC;
out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_Position;
uniform mat4 u_MVP;
uniform mat4 u_ModelMatrix;
uniform int u_Wireframe;
uniform unsigned int u_RenderMode;


void main()
{
	
if(u_Wireframe != 0)
{
	gl_Position = (u_MVP * position) - vec4(0.0, 0.0, 0.0005, 0.0); //Offset to avoid Z fighting.
}
else
{
	gl_Position = u_MVP * position;
}
	
v_Position = vec3(gl_Position);
v_Normal = vec3(u_ModelMatrix * normal);
v_TexCoord = texCoord;
}



#shader fragment
#version 330 core

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_BC;

uniform int u_Wireframe;
uniform vec3 u_Color;
uniform int u_DrawMode;


//Forward Lights
uniform vec3 u_LightPosA;
uniform vec3 u_LightColorA;
//uniform vec3 u_LightPosB;
//uniform vec3 u_LightColorB;
//uniform vec3 u_LightPosC;
//uniform vec3 u_LightColorC;


uniform float u_SpecIntensity;
uniform float u_SpecFalloff;
uniform vec3 u_CameraPos;

uniform float u_Near;
uniform float u_Far;

uniform sampler2D u_Texture;
uniform samplerCube u_Sky;
uniform int u_DrawReflections;
uniform int u_DrawTextures;


out vec4 FragColor;


void main()
{
	
	vec3 albedo;
	
	if(u_DrawTextures == 1)
	{
		albedo = vec3(texture(u_Texture, v_TexCoord));	
	}
	else
	{
		albedo = u_Color;
		
	}
	
	
	if(u_Wireframe != 0)
	{
		FragColor = vec4(1.0);
	}

	// Blinn
	else if (u_DrawMode == 0)
    {
			vec3 lightAngle = normalize(u_LightPosA);
			
			 // Specular
			vec3 veiwDir = normalize(v_Position - u_CameraPos);
			vec3 norm = normalize(v_Normal);	
			vec3 reflectDir = normalize(reflect(-lightAngle, norm));
			float dirDot = max(dot(-veiwDir, reflectDir), 0.0);
			float spec = pow(dirDot, max(u_SpecFalloff, 1.0));
			vec3 specular = u_SpecIntensity * spec * u_LightColorA;
			 
			 // Reflection
			vec3 I = normalize(v_Position - u_CameraPos);
			vec3 R = reflect(I, normalize(v_Normal));
			
			
			vec3 reflectColor = vec3(texture(u_Sky, R).rgb) * .8;
			if(u_DrawReflections != 1)
			{
				reflectColor = vec3(0.0);
			}
	

			 // Diffuse
			
			float diff = max(dot(lightAngle, norm), 0.0); // Prevent Negative Values
			vec3 diffuse = diff * u_LightColorA;
			vec3 ambient = vec3(0.25f);

			//FragColor = vec4(reflectColor, 1.0);
			FragColor  = vec4(((ambient + diffuse + specular + reflectColor) * albedo) / 2.0, 1.0);	
			//FragColor = vec4(albedo * vec3(diff), 1.0);
    }
	
	
	// Reflect
	else if(u_DrawMode == 4)
	{
	vec3 I = normalize(v_Position - u_CameraPos);
	vec3 R = reflect(I, normalize(v_Normal));
	FragColor = vec4(texture(u_Sky, R).rgb, 1.0);
	}
	else
	{	
	FragColor = vec4(u_Color, 1.0);	
	}
		
	
	
	//FragColor =texture(skybox, vec3(0.5));
	//FragColor = vec4(u_Color, 1.0);
}

