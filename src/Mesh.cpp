#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureID> textures, std::string meshName) 
	:mVertices(vertices), mIndices(indices), mTextures(textures), MeshNode(meshName)
{
	LOG_DEBUG("Mesh Child node {}", mName.c_str());
	setupMesh();
}

Mesh::~Mesh()
{
	// Cleanup Cache
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
	glDeleteVertexArrays(1, &mVAO);
	printf("\n Destroying %s \n", mName.c_str());

}

void Mesh::setupMesh()
{
	if (!mVertices.size())
	{
		LOG_ERROR("Mesh is not loaded!");
		return;
	}
	GLCall(glGenVertexArrays(1, &mVAO));  // Vertex Array	
	GLCall(glGenBuffers(1, &mVBO));       // Vertex Buffer
	GLCall(glGenBuffers(1, &mEBO));       // Index Buffer


	GLCall(glBindVertexArray(mVAO));
	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	// Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	// Vertex Positions 
	GLCall(glEnableVertexAttribArray(0));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	
	// Normals
	GLCall(glEnableVertexAttribArray(1));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// vertex texture coords
	GLCall(glEnableVertexAttribArray(2));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// barycentric coords
	GLCall(glEnableVertexAttribArray(3));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BaryCentricCoords));

	// Tangents
	GLCall(glEnableVertexAttribArray(4));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangents));

	// Bitangents
	GLCall(glEnableVertexAttribArray(5));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BiTangents));


	// Unbind cleanup
	glBindVertexArray(0);

}

void Mesh::draw()
{
	if (!mVAO)
	{
		return;
	}
	GLCall(glBindVertexArray(mVAO););
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);


	glBindVertexArray(0);
}

void Mesh::draw(GLenum pMode)
{

	if (!mVAO)
	{
		return;
	}
	GLCall(glBindVertexArray(mVAO););
	glDrawElements(pMode, mIndices.size(), GL_UNSIGNED_INT, nullptr);
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




TransformNode::TransformNode(const glm::mat4* pProg, const glm::mat4* pView)
	: mProj(pProg), mView(pView)
{
	setUpManip();
	mDrawManip = true;
}

void TransformNode::setModel(glm::mat4 pModel)
{
	mModel = pModel;
}

void TransformNode::setPosition(glm::vec3 pPosition)
{
	mModel = glm::translate(glm::mat4(1.0), pPosition);
}

void TransformNode::draw()
{
	if (mView == nullptr)
	{
		LOG_ERROR("mView is Null");
		return;
	}

	if (mProj == nullptr)
	{
		LOG_ERROR("mProj is Null");
		return;
	}

	if (mDrawManip)
	{
		drawManip();
	}


}

void TransformNode::setUpManip()
{
	mManipSize = 1.0f;
	mCenter = glm::vec4(0.0, 0.0, 0.0, 1.0);
	mX = glm::vec4(1.0, 0.0, 0.0, 1.0);
	mY = glm::vec4(0.0, 1.0, 0.0, 1.0);
	mZ = glm::vec4(0.0, 0.0, 1.0, 1.0);

	mModel = glm::mat4(1.0); // Identity Matrix
}

void TransformNode::drawManip()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glm::mat4 proj = *mProj;
	glm::mat4 view = *mView;

	glm::mat4 scale = glm::scale(mModel, glm::vec3(mManipSize));
	
	glm::mat4 mvp = proj * view * mModel;
	glm::vec4 center(mvp * mCenter);
	glm::vec4 x(mvp * mX);
	glm::vec4 y(mvp * mY); 
	glm::vec4 z(mvp * mZ);
	
	
	// X Handle
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex4f(center.x, center.y, center.z, center.w);
	glVertex4f(x.x, x.y, x.z, x.w);
	glEnd();
	
	// Y Handle
	glBegin(GL_LINES);
	glColor3f(0.0, 1.0, 0.0);
	glVertex4f(center.x, center.y, center.z, center.w);
	glVertex4f(y.x, y.y, y.z, y.w);
	glEnd();

	// Z Handle
	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 1.0);
	glVertex4f(center.x, center.y, center.z, center.w);
	glVertex4f(z.x, z.y, z.z, z.w);
	glEnd();



}

MeshNode::MeshNode(std::string pName)
	: mName(pName)
{


}
