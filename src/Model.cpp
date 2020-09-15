#include "Model.h"


Model::Model(const char *path)
{
	// Clean up previously loaded model. 
	if (mLoaded)
	{
		LOG_DEBUG("Previous Model was loaded. Need to cleanup first");
	}
	LoadModel(path);
}


Model::Model()
{
	mLoaded = false;
	return;
}

void Model::Draw()
{
	if (!mLoaded)
	{
		return;
	}

	for (unsigned int i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i]->draw();
	}
}

void Model::Draw(GLenum pMode)
{
	if (!mLoaded)
	{
		return;
	}

	for (unsigned int i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i]->draw(pMode);
	}



}

Model::~Model()
{
	UnloadModel();
}

void Model::LoadModel(std::string path)
{
	UnloadModel();

	Assimp::Importer importer;
	//const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ADDIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
	mLoaded = true;
}

void Model::UnloadModel()
{

	mLoaded = false;
	// Clear data
	for each (Mesh* m in mMeshes)
	{
		delete(m);
	}
	mMeshes.clear();
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		LOG_DEBUG("loading Mesh: {}", mesh->mName.C_Str());
		//mMeshes.push_back(processMesh(mesh, scene));
		processMesh(mesh, scene);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TextureID> textures;
	std::vector<glm::vec3> barycentric = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
	std::vector<glm::vec3>tangents;
	std::vector<glm::vec3> bitangents;

	unsigned int currentIdx = 0;

	LOG_DEBUG("Mesh Vertex Count: {}", mesh->mNumVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// Process vertex positions, normals and tex coords

		// VERTEX
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// NORMAL
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;


		// Tex Coords

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		
		vertex.BaryCentricCoords = barycentric[currentIdx];
		if (currentIdx >= barycentric.size() - 1)
		{
			currentIdx = 0;
		}
		else
		{
			currentIdx += 1;
		}

		vertices.push_back(vertex);
	}


	// Import Tangents
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		vertices[i].Tangents.x = mesh->mTangents[i].x;
		vertices[i].Tangents.y = mesh->mTangents[i].y;
		vertices[i].Tangents.y = mesh->mTangents[i].z;

		vertices[i].BiTangents.x = mesh->mBitangents[i].x;
		vertices[i].BiTangents.y = mesh->mBitangents[i].y;
		vertices[i].BiTangents.y = mesh->mBitangents[i].z;


	}


	// Indices 
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}


	//if (indices.size() != mesh->mNumVertices)
	//{
	//	LOG_ERROR("Mesh vertex size != indices size, vertex count {} indices count {}", mesh->mNumVertices, indices.size());

	//}
	LOG_DEBUG("Indices Size {}", indices.size());





	// Calculate Tangents Indices
	//for (unsigned int i = 0; i < indices.size() / 3; i++)
	//{

	//	int v1 = indices[i * 3];
	//	int v2 = indices[(i * 3) + 1];
	//	int v3 = indices[(i * 3) + 2];


	//	glm::vec3 pos1 = vertices[v1].Position;
	//	glm::vec3 pos2 = vertices[v2].Position;
	//	glm::vec3 pos3 = vertices[v3].Position;

	//	glm::vec2 uv1 = vertices[v1].TexCoords;
	//	glm::vec2 uv2 = vertices[v2].TexCoords;
	//	glm::vec2 uv3 = vertices[v3].TexCoords;

	//	glm::vec3 edge1 = pos2 - pos1;
	//	glm::vec3 edge2 = pos3 - pos1;

	//	glm::vec2 deltaUV1 = uv2 - uv1;
	//	glm::vec2 deltaUV2 = uv3 - uv1;

	//	glm::vec3 tangent;
	//	glm::vec3 bitangent;

	//	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	//	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	//	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	//	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	//	bitangent.x = f * (-deltaUV2.x * edge1.x - deltaUV1.x * edge2.x);
	//	bitangent.y = f * (-deltaUV2.x * edge1.y - deltaUV1.x * edge2.y);
	//	bitangent.z = f * (-deltaUV2.x * edge1.z - deltaUV1.x * edge2.z);


	//	// Set Vertices tangent and bitangent. 
	//	vertices[v1].Tangents = tangent;
	//	vertices[v1].BiTangents = bitangent;

	//	vertices[v2].Tangents = tangent;
	//	vertices[v2].BiTangents = bitangent;

	//	vertices[v3].Tangents = tangent;
	//	vertices[v3].BiTangents = bitangent;

	//}












	//// Calculate Tangents Vertices
	//for (unsigned int i = 0; i < mesh->mNumVertices / 3; i++)
	//{
	//	glm::vec3 pos1 = vertices[i * 3].Position;
	//	glm::vec3 pos2 = vertices[(i * 3) + 1].Position;
	//	glm::vec3 pos3 = vertices[(i * 3) + 2].Position;

	//	glm::vec2 uv1 = vertices[i * 3].TexCoords;
	//	glm::vec2 uv2 = vertices[(i * 3) + 1].TexCoords;
	//	glm::vec2 uv3 = vertices[(i * 3) + 2].TexCoords;

	//	glm::vec3 edge1 = pos2 - pos1;
	//	glm::vec3 edge2 = pos3 - pos1;

	//	glm::vec2 deltaUV1 = uv2 - uv1;
	//	glm::vec2 deltaUV2 = uv3 - uv1;

	//	glm::vec3 tangent;
	//	glm::vec3 bitangent;

	//	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	//	
	//	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	//	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	//	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	//	
	//	bitangent.x = f * (-deltaUV2.x * edge1.x - deltaUV1.x * edge2.x);
	//	bitangent.y = f * (-deltaUV2.x * edge1.y - deltaUV1.x * edge2.y);
	//	bitangent.z = f * (-deltaUV2.x * edge1.z - deltaUV1.x * edge2.z);


	//	// Set Vertices tangent and bitangent. 
	//	vertices[i * 3].Tangents = tangent;
	//	vertices[i * 3].BiTangents = bitangent;

	//	vertices[(i * 3) + 1].Tangents = tangent;
	//	vertices[(i * 3) + 1].BiTangents = bitangent;

	//	vertices[(i * 3) + 2].Tangents = tangent;
	//	vertices[(i * 3) + 2].BiTangents = bitangent;

	//}



	Mesh* pMesh = new Mesh(vertices, indices, textures, mesh->mName.C_Str());
	mMeshes.push_back(pMesh);

	//return Mesh(vertices, indices, textures, mesh->mName.C_Str());


}
