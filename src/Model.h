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

	~Model();

private:
	std::vector<Mesh> mMeshes;
	std::string directory;

	bool mLoaded = false;


	void loadModel(std::string path);

	void processNode(aiNode *node, const aiScene *scene);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<TextureID> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);


};


