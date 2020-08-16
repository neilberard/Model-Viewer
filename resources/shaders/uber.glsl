#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // Location is int id of the vertex attribute pointer TODO: Change these to vec3
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
    v_BC = barycentric;

};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_BC;

uniform sampler2D u_Texture;
uniform int u_Wireframe;
uniform vec3 u_Color;
uniform int u_DrawMode;
uniform samplerCube skybox;


//Forward Lights
uniform vec3 u_LightPosA;
uniform vec3 u_LightColorA;

uniform vec3 u_LightPosB;
uniform vec3 u_LightColorB;

uniform vec3 u_LightPosC;
uniform vec3 u_LightColorC;
uniform float u_SpecIntensity;
uniform float u_SpecFalloff;
uniform vec3 u_CameraPos;

uniform float u_Near;
uniform float u_Far;


float LinearizeDepth(float depth)
{
float z = (depth * 2.0) - 1.0;
return (2.0 * u_Near * u_Far) / (u_Far + u_Near - z * (u_Far - u_Near));
}


void main()
{    // WIREFRAME
    if(u_Wireframe != 0)
    {
        color = vec4(u_Color, 1.0);
		return;
    }

    // NORMAL
    if (u_DrawMode == 1)
    {
       color = vec4(v_Normal, 1.0);
	   normalize(color);
    }

    // TEXTURE FLAT
    if (u_DrawMode == 2)
    {
		vec4 texColor = texture(u_Texture, v_TexCoord);
		color = texColor;
		return;
    }

    // ZDEPTH
    if (u_DrawMode == 3)
    {
    float depth = LinearizeDepth(gl_FragCoord.z) / u_Far;
    color = vec4(vec3(depth), 1.0) + vec4(0.1, 0.1, 0.1, 1.0);
    }

    // DIFFUSE
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
     
    
    float diff = max(dot(lightAngle, norm), 0.0); // Prevent Negative Values
    //color = vec4(0.5, 0.5, 0.5, 1.0) * dot(vec4(0.0, 1.0, 0.0, 1.0), v_Normal);
    vec3 diffuse = diff * u_LightColorA;
    vec3 ambient = vec3(0.1f);
	
	if(dirDot > 1.0f)
	{
		color = vec4(1.0, 0.0, 0.0, 1.0);	
	}
	else
	{
		//color = vec4(1.0, 0.0, 0.0, 1.0);
		color = vec4((ambient + diffuse + specular) * u_Color, 1.0);	
	}

	//color = vec4(specular, 1.0);
    }
   
	// FLAT TEXTURES
    else if (u_DrawMode == 4)
    {
	vec3 I = normalize(v_Position - u_CameraPos);
	vec3 R = reflect(I, normalize(v_Normal));
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
	return;
    }
	
	// UV COORDS
	else if (u_DrawMode == 5)
	{
		color = vec4(v_TexCoord.x, v_TexCoord.y, 0.0, 1.0);	
	}
    




    //if(v_BC.x < 0.05 || v_BC.y < 0.05 || v_BC.z < 0.05)
    //{
    //    color = vec4(0.0, 0.0, 0.0, 1.0f);
    //}
    //else
    //{

    //    color = vec4(v_Normal[0], v_Normal[1], v_Normal[2], 0.0f);
    
    //}


    //color = vec4(v_BC[0], v_BC[1], v_BC[2], 1.0f);
    //color = vec4(v_Position[3], v_TexCoord[0],v_TexCoord[1], 1.0f);
    //vec4 texColor = texture(u_Texture, v_TexCoord);
    //color = u_Color;
    //color = v_Normal;
    //if(u_Color[3] > 0.0f)
    //{
    //color = u_Color;
    //}
    //else
    //{
    //color = vec4(v_Normal[0], v_Normal[1], v_Normal[2], 0.0f);
    //}
    //color = vec4(0.5f, 0.0f, 0.0f, 0.0f);  
};

