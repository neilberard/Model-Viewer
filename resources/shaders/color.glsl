#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // Location is int id of the vertex attribute pointer TODO: Change these to vec3
layout (std140) uniform uBlock
{
    mat4 uProjection;
    mat4 uView;
	mat4 uModel;
};

//uniform mat4 uModel;
uniform vec3 uOffset;


void main()
{
gl_Position = (uProjection * uView * uModel * position) -vec4(uOffset, 0.0);
}



#shader fragment
#version 330 core


//Forward Lights
uniform vec3 uColor;

out vec4 FragColor;


void main()
{
	FragColor = vec4(uColor, 1.0);
}