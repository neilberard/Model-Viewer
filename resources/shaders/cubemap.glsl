#shader vertex
#version 330 core

layout(location = 0) in vec3 position; 

layout (std140) uniform uBlock
{
    mat4 uProjection;
    mat4 uView;
};


out vec3 TexCoords;

void main()
{
	TexCoords = vec3(position);
	gl_Position = uProjection * uView * vec4(TexCoords, 0.01);
}


#shader fragment
#version 330 core

out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube uSky;

void main()
{
	FragColor = texture(uSky, TexCoords);



}

