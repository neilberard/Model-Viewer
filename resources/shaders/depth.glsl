#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // Location is int id of the vertex attribute pointer
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 barycentric;


out vec3 v_BC;
out vec2 v_TexCoord;
out vec4 v_Normal;
out vec4 v_Position;
uniform mat4 u_MVP;
uniform mat4 u_ModelMatrix;
uniform int u_Wireframe;
uniform unsigned int u_RenderMode; 


layout (std140) uniform uBlock
{
    mat4 uProjection;
    mat4 uView;
};





void main()
{
    //positionV4 = vec4(position[0], position[1], position[2], 0.0f);
    //normalV4 = vec4(normal[0], normal[1], normal[2], 0.0f); 
    
    
    if(u_Wireframe != 0)
    {
        gl_Position = (u_MVP * position) - vec4(0.0, 0.0, 0.0005, 0.0);
    }
    else
    {
        gl_Position = u_MVP * position;
    
    }

    v_TexCoord = texCoord;
    v_Normal = u_ModelMatrix * normal;
    v_Position = position;

    v_BC = barycentric;

};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
in vec4 v_Normal;
in vec4 v_Position;
in vec3 v_BC;

uniform int u_Wireframe;
uniform vec4 u_Color;

//uniform sampler2D u_Texture;

void main()
{

    if(u_Wireframe != 0)
    {
        color = vec4(1.0, 1.0, 1.0, 1.0f);
    }

    else
    {
       color = v_Normal;
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

