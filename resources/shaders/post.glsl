#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // 1.0 is auto added when converting to vec4 from 3
layout(location = 2) in vec2 texCoord;
out vec2 v_TexCoord;


void main()
{
gl_Position = position;
v_TexCoord = texCoord;

}

#shader fragment
#version 330 core


in vec2 v_TexCoord;
out vec4 FragColor;
uniform sampler2D u_Texture;
uniform float u_Near;
uniform float u_Far;

const float offset = 1.0f / 300.0f;

/* const float u_Near = 0.3f;
const float u_Far = 5.0f; */


float LinearizeDepth(float depth)
{
float z = (depth * 2.0) - 1.0;
return (2.0 * u_Near * u_Far) / (u_Far + u_Near - z * (u_Far - u_Near));
}



void main()
{

  vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
	
/* 	float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
); */

	
	
	
    float depth = LinearizeDepth(texture(u_Texture, v_TexCoord).z) / u_Far;
    vec4 color = vec4(vec3(depth), 1.0) + vec4(0.1, 0.1, 0.1, 1.0);


	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(u_Texture, v_TexCoord.st + offsets[i]));
	}	
	
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++){
		col += sampleTex[i] * kernel[i];
	}

	depth = texture(u_Texture, v_TexCoord).r;
	FragColor = vec4(1.0) - vec4(vec3(depth), 1.0);
	//FragColor = vec4(vec3(texture(u_Texture, v_TexCoord).z), 1.0);
	//FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
	//FragColor = vec4(col, 1.0);
	//FragColor = vec4(1.0) - texture(u_Texture, v_TexCoord);

}

