#shader vertex
#version 330 core

layout(location = 0) in vec3 position; 

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_MVP;

out vec3 TexCoords;

void main()
{
	//Normalize Z
	TexCoords = vec3(position);
	//gl_Position = u_Projection * u_View * position;
	gl_Position = u_MVP * vec4(TexCoords, 0.01);
}


#shader fragment
#version 330 core

out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, TexCoords);
	//FragColor = vec4(TexCoords.x, TexCoords.y, 0.0, 1.0) + texture(skybox, TexCoords);
}

