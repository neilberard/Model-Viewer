#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	vec4 VertexNormal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int uDrawTexture;
uniform int uDrawNormal;


void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
	//TBN = transpose(mat3(T, B, N)); 

	
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
	vs_out.VertexNormal = projection * view * model * vec4(aNormal, 1.0);
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}



#shader fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	vec4 VertexNormal;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int uDrawTexture;
uniform int uDrawNormal;
uniform int uDrawMode;

void main()
{           
     // obtain normal from normal map in range [0,1]
	vec3 normal = vec3(0.5, 0.5, 1.0);
	 
	if (uDrawNormal == 1)
	{
		
		normal = texture(normalMap, fs_in.TexCoords).rgb;	
	}
	
	if (uDrawMode == 1)
	{
		FragColor = fs_in.VertexNormal;
		return;
	}


	// transform normal vector to range [-1,1]
	
	
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
	//normal.xy = normalize(normal.xy * 2.0 - 1.0);  
   
    // get diffuse color
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
	color = vec3(0.5,0.5,0.5);
	
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    //vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	vec3 lightDir = normalize(fs_in.TangentLightPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
	
	
	
}