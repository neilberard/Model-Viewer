#pragma  once

#include "Debugging.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "Log.h"
#include <string>
#include <vector>



struct Vertex {
	glm::vec4 Position;
	glm::vec4 Normal;
	glm::vec2 TexCoords;
	glm::vec3 BaryCentricCoords;
};

struct TextureID {
	unsigned int id;
	std::string type;
};

class Mesh {
public:
	//Shader mShader;
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<TextureID> mTextures;

	bool mDebug = true;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureID> textures);
	~Mesh();
	void draw();

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();

};

class LightCube {
public:
	float scale;

	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<TextureID> mTextures;
	LightCube(float pSize=1.0f);
	~LightCube();
	void draw(GLenum pMode = GL_FILL);
private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();

};
