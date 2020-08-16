#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // Location is int id of the vertex attribute pointer TODO: Change these to vec3


uniform mat4 u_MVP;

void main()
{
gl_Position = u_MVP * position;
}



#shader fragment
#version 330 core


//Forward Lights
uniform vec3 u_LightColorA;

out vec4 FragColor;


void main()
{
	FragColor = vec4(u_LightColorA, 1.0);
}

