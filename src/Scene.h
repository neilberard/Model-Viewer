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

	SceneContext(int pScreenWidth, int pScreenHeight);
	bool drawLights;
	bool drawGeo;


private:
	int mScreenWidth, mScreenHeight;
	std::vector<Shader*>shaders;


};

