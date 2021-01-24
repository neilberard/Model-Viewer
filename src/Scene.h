#pragma  once
#include "Log.h"
#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include <gl/glew.h>


class SceneContext
{
public:

	SceneContext();
	~SceneContext();
private:

	std::vector<Shader*>shaders;


};

