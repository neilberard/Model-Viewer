#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT 
{
    vec3 FragPos;
    vec2 TexCoords;
    vec4 FragPosLightSpace;

} vs_out;






out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}


#shader fragment
#version 330 core

