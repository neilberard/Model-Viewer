
#ifndef MESH_H
#define MESH_H

#include "Debugging.h"
#include "glm/glm.hpp"
#include <string>
#include <vector>
#include "Debugging.h"

//Logger Log;

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
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<TextureID> mTextures;

	bool mDebug = true;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureID> textures);
	~Mesh();
	void draw(GLenum pMode=GL_FILL);

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();

};

#endif