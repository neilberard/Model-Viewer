#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Log.h"


class Model
{
public:
	Model();
	Model(const char *path);

	void Draw();
	void Draw(GLenum pMode);
	void LoadModel(std::string path);
	void UnloadModel();

	~Model();

private:
	std::vector<Mesh*>mMeshes;
	std::vector<TransformNode>mTransformNodes;
	std::string directory;

	bool mLoaded = false;

	void processNode(aiNode *node, const aiScene *scene);

	void processMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<TextureID> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);


};


