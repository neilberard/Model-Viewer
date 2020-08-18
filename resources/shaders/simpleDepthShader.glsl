#shader vertex
#version 330 core
layout(location = 0) in vec4 position;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;


void main()
{
	gl_Position = u_LightSpaceMatrix * u_Model * position;
}

#shader fragment
#version 330 core

void main()
{
// No processing required for the Fragment.
}
