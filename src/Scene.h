#pragma  once
#include "Log.h"
#include <vector>
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include <gl/glew.h>
#include <memory>


class SceneContext
{
public:

	SceneContext();
	~SceneContext();

	void reloadShaders();
	void addShader(Shader &pShader);
	Shader* createShader(const char* path);


private:

	std::vector<Shader*>mShaders;
	std::vector<Texture*>mTextures;

};

