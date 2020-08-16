#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> pVertices, std::vector<unsigned int> pIndices, std::vector<TextureID> pTextures)
	:mVertices(pVertices), mIndices(pIndices), mTextures(pTextures)
{
	setupMesh();
}

Mesh::~Mesh()
{
	//Cleanup
	//glBindVertexArray(0);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	////glDeleteVertexArrays(1, &VAO); //Deleting this will prevent the successfull generation of a new VAO 
	//mVertices.clear();
	//mIndices.clear();
	//mTextures.clear();

	//VAO = 0;
}

void Mesh::setupMesh()
{
	if (!mVertices.size())
	{
		LOG_ERROR("Mesh is not loaded!");
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	// Normals
	GLCall(glEnableVertexAttribArray(1));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// vertex texture coords
	GLCall(glEnableVertexAttribArray(2));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// barycentric coords
	GLCall(glEnableVertexAttribArray(3));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BaryCentricCoords));

	// Unbind cleanup
	glBindVertexArray(0);

}

void Mesh::draw()
{
	if (!VAO)
	{
		return;
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, mVertices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

SimpleCube::SimpleCube(float pSize, bool pReverseNormals/*=false*/)
	:mScale(pSize)

{
	setupMesh();


}


SimpleCube::~SimpleCube()
{


}

void SimpleCube::draw(GLenum pMode /*= GL_FILL*/)
{
	//LOG_INFO("Rendering Cube with polycount {}, assimp polycount {}", this->mVertices.size(), mVertices.size());
	glBindVertexArray(VAO);
	// I'm not sure why indices.size() works and not mVertices.size() TODO: invesitgate this!
	//LOG_INFO("Drawing simple cube {}", mIndices.size());
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

}

void SimpleCube::setupMesh()
{

	std::vector<float>positions = {
	-1.0f, -1.0f, -1.0f, // 0
	-1.0f,  1.0f, -1.0f, // 1
	 1.0f,  1.0f, -1.0f, // 2
	 1.0f, -1.0f, -1.0f, // 3
	-1.0f, -1.0f,  1.0f, // 4
	-1.0f,  1.0f,  1.0f, // 5
	 1.0f,  1.0f,  1.0f, // 6
	 1.0f, -1.0f,  1.0f  // 7
	};

	mIndices = {
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};

	//LOG_DEBUG("MIndices count {}", mIndices.size());

	for (unsigned int i = 0; i < positions.size() / 3; i++)
	{
		Vertex vertex;
		// Process vertex positions, normals and tex coords

		// VERTEX
		glm::vec3 vector;
		vector.x = positions[i * 3] * mScale;
		vector.y = positions[(i * 3) + 1] * mScale;
		vector.z = positions[(i * 3) + 2] * mScale;
		vertex.Position = vector;

		//LOG_INFO("Adding Vertex {} x:{} y:{} z:{}", i, vector.x, vector.y, vector.z);

		mVertices.push_back(vertex);
	}

	if (!mVertices.size())
	{
		LOG_ERROR("Mesh is not loaded!");
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Unbind cleanup
	glBindVertexArray(0);



}





/// Plane 

SimplePlane::SimplePlane(float pSize, bool pReverseNormals /*= false*/)
	:mScale(pSize)
{
	setupMesh();

}



SimplePlane::~SimplePlane()
{


}

void SimplePlane::Draw(GLenum pMode /*= GL_FILL*/)
{
	//LOG_INFO("Rendering Cube with polycount {}, assimp polycount {}", this->mVertices.size(), mVertices.size());
	glBindVertexArray(VAO);
	// I'm not sure why indices.size() works and not mVertices.size() TODO: invesitgate this!
	//LOG_INFO("Drawing simple cube {}", mIndices.size());
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

}

void SimplePlane::setupMesh()
{

	std::vector<float>positions = {
	-1.0f,  1.0f, 0.0f, // 0
	-1.0f, -1.0f, 0.0f, // 1
	 1.0f, -1.0f, 0.0f, // 2
	-1.0f,  1.0f, 0.0f, // 3
	 1.0f, -1.0f, 0.0f, // 4
	 1.0f, 1.0f, 0.0f   // 5
	};

	std::vector<float>uvs = {
	0.0f, 1.0f, // 0
	0.0f, 0.0f, // 1
	1.0f, 0.0f, // 2
	0.0f, 1.0f, // 3
	1.0f, 0.0f, // 4
	1.0f, 1.0f  // 5
	};

	mIndices = {
		0, 1, 2, 3, 4, 5
	};

	//LOG_DEBUG("MIndices count {}", mIndices.size());

	for (unsigned int i = 0; i < positions.size() / 3; i++)
	{
		Vertex vertex;
		// Process vertex positions, normals and tex coords

		// VERTEX
		glm::vec3 vector;
		vector.x = positions[i * 3] * mScale;
		vector.y = positions[(i * 3) + 1] * mScale;
		vector.z = positions[(i * 3) + 2] * mScale;
		vertex.Position = vector;
		//LOG_INFO("Adding Vertex {} x:{} y:{} z:{}", i, vector.x, vector.y, vector.z);

		// UV
		vertex.TexCoords = glm::vec2(uvs[i * 2], uvs[(i * 2) + 1]);
		mVertices.push_back(vertex);
	}

	if (!mVertices.size())
	{
		LOG_ERROR("Mesh is not loaded!");
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// vertex texture coords
	GLCall(glEnableVertexAttribArray(2));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));


	// Unbind cleanup
	glBindVertexArray(0);



}

