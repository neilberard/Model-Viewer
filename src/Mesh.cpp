#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> pVertices, std::vector<unsigned int> pIndices, std::vector<TextureID> pTextures)
{
	this->mVertices = pVertices;
	this->mIndices = pIndices;
	this->mTextures = pTextures;
	setupMesh();
}

Mesh::~Mesh()
{
	glBindVertexArray(0);
	//GLCall(glDeleteVertexArrays(1, &VAO));
	printf("Destroy Mesh");
}

void Mesh::setupMesh()
{
	if (!mVertices.size())
	{
		printf("\nMesh is not loaded!");
		return;
	}

	GLCall(glGenVertexArrays(1, &VAO));  // Vertex Array	
	GLCall(glGenBuffers(1, &VBO));       // Vertex Buffer
	GLCall(glGenBuffers(1, &EBO));       // Index Buffer


	GLCall(glBindVertexArray(VAO));
	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	// Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	// Vertex Positions 
	GLCall(glEnableVertexAttribArray(0));
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	// Normals
	GLCall(glEnableVertexAttribArray(1));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// vertex texture coords
	GLCall(glEnableVertexAttribArray(2));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// barycentric coords
	GLCall(glEnableVertexAttribArray(3));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BaryCentricCoords));

	// Unbind cleanup
	glBindVertexArray(0);
	//Log.log("\n Buffers are binded!\n");

}

void Mesh::draw(GLenum pMode/*=GL_FILL*/)
{
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT, pMode);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glDrawElements(GL_TRIANGLES, mVertices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);


}



