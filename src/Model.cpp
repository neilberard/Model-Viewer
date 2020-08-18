#include "Model.h"


Model::Model(const char *path)
{
	// Clean up previously loaded model. 
	if (mLoaded)
	{
		LOG_DEBUG("Previous Model was loaded. Need to cleanup first");
	}
	loadModel(path);
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
		mMeshes[i].draw();
	}
}

Model::~Model()
{
}

void Model::loadModel(std::string path)
{
	mLoaded = false;
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ADDIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
	mLoaded = true;
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		LOG_DEBUG("loading Mesh: {}", mesh->mName.C_Str());
		mMeshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TextureID> textures;
	std::vector<glm::vec3> barycentric = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
	unsigned int currentIdx = 0;

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

	// Indices 
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(vertices, indices, textures, mesh->mName.C_Str());


}
