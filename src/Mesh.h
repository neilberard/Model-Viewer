#pragma once

#include "Debugging.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Log.h"
#include <string>
#include <vector>



struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 BaryCentricCoords;
	glm::vec3 Tangents;
	glm::vec3 BiTangents;
};

struct TextureID {
	unsigned int id;
	std::string type;
};



class MeshNode {
public:
	MeshNode(std::string pName);
	std::string mName;
	int mId;
	bool mSelected;
};



class Mesh: public MeshNode {
public:
	//Shader mShader;
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<TextureID> mTextures;

	bool mDebug = true;

	// For shadows. If this is considered ground or floor.
	bool mFloor = false;
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureID> textures, std::string meshName);
	~Mesh();
	void draw();
	void draw(GLenum pMode);

private:
	unsigned int mVAO, mVBO, mEBO;
	glm::mat4 mModel;
	Shader* mShader;

	void setupMesh();
};




class TransformNode {
	// Contains methods for selecting and transforming a mesh.
public:
	bool mSelected;
	bool mDrawManip;
	float mManipSize;

	TransformNode(const glm::mat4* pProj, const glm::mat4* pView);
	TransformNode(const Mesh* pMesh, Shader* pShader, const glm::mat4* pProg, const glm::mat4* pView, unsigned int pId=0);
	
	// Getters
	glm::vec3 position();

	// Setters
	void setModel(glm::mat4 pModel);
	void setPosition(glm::vec3 pPosition);
	void setRotation(glm::vec3 pRotation);
	
	void draw();
	void draw(Shader* overrideShader);


private:
	void setUpManip();
	void drawManip();

	const glm::mat4* mView;
	const glm::mat4* mProj;



	unsigned int mId;
	glm::mat4 mModel;
	Mesh* mMesh;
	Shader* mShader;

	// Handles 
	glm::vec4 mX;
	glm::vec4 mY;
	glm::vec4 mZ;
	glm::vec4 mCenter;


};




class SimpleCube {
public:
	float mScale;
	glm::mat4 mModel;
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

namespace
{

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	void renderQuad()
	{
		if (quadVAO == 0)
		{
			// positions
			glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
			glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
			glm::vec3 pos3(1.0f, -1.0f, 0.0f);
			glm::vec3 pos4(1.0f, 1.0f, 0.0f);
			// texture coordinates
			glm::vec2 uv1(0.0f, 1.0f);
			glm::vec2 uv2(0.0f, 0.0f);
			glm::vec2 uv3(1.0f, 0.0f);
			glm::vec2 uv4(1.0f, 1.0f);
			// normal vector
			glm::vec3 nm(0.0f, 0.0f, 1.0f);

			// calculate tangent/bitangent vectors of both triangles
			glm::vec3 tangent1, bitangent1;
			glm::vec3 tangent2, bitangent2;
			// triangle 1
			// ----------
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

			// triangle 2
			// ----------
			edge1 = pos3 - pos1;
			edge2 = pos4 - pos1;
			deltaUV1 = uv3 - uv1;
			deltaUV2 = uv4 - uv1;

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


			bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


			float quadVertices[] = {
				// positions            // normal         // texcoords  // tangent                          // bitangent
				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
				pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
				pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
			};
			// configure plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}
