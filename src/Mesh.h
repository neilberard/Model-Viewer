#pragma  once

#include "Debugging.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "Log.h"
#include <string>
#include <vector>



struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
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

class SimpleCube {
public:
	float mScale;
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<TextureID> mTextures;
	SimpleCube(float pSize, bool pReverseNormals=false);
	~SimpleCube();
	void draw(GLenum pMode = GL_FILL);
	void setupMesh();
private:
	unsigned int VAO, VBO, EBO;
};


class SimplePlane {

public:
	float mScale;
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<TextureID> mTextures;
	SimplePlane(float pSize, bool pReverseNormals = false);
	~SimplePlane();
	void Draw(GLenum pMode = GL_FILL);
	void setupMesh();
private:
	unsigned int VAO, VBO, EBO;

};
