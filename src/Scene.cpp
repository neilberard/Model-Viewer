#include "Scene.h"
#include "Log.h"



SceneContext::SceneContext()
{


}

SceneContext::~SceneContext()
{
	LOG_INFO("Deleting SceneContext. Goodbye!");

}

void SceneContext::reloadShaders()
{
	//for (int i = 0; i < mShaders.size(); i++)
	//{
	//	LOG_INFO("Reloading Shader {}", i);
	//	//shader->Reload();
	//	
	//}
}

void SceneContext::addShader(Shader &pShader)
{
	mShaders.push_back(&pShader);

}

Shader* SceneContext::createShader(const char* path)
{
	Shader* shader = new Shader(path);
	return shader;

}
