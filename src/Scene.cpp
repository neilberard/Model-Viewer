#include "Scene.h"


SceneContext::SceneContext(int pScreenWidth, int pScreenHeight)
	:mScreenWidth(pScreenWidth), mScreenHeight(pScreenHeight)
{

}

void SceneContext::Init()
{
	//Log::set_level();
	LOG_INFO("Initialized Log!");

	/* Initialize the library */
	if (!glfwInit())
	{
		LOG_ERROR("GLFW failed to intialize!");
		return;
	}

	/* Create a windowed mode window and its OpenGL context */
	mWindow = glfwCreateWindow(mScreenWidth, mScreenHeight, "NB Model Viewer", NULL, NULL);
	if (!mWindow)
	{
		LOG_INFO("Closing Window");
		glfwTerminate();
		return;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(mWindow);

	//--------------------- CALLBACKS ----------------------- //
	//glfwSetKeyCallback(mWindow, KeyCallback);
	//glfwSetWindowSizeCallback(mWindow, WindowSizeCallback);
	//glfwSetCursorPosCallback(mWindow, MouseCallback);
	//glfwSetScrollCallback(mWindow, ScrollCallback);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glewInit() != GLEW_OK)
	{
		LOG_CRITICAL("Glew failed to initialize!");

	}


	mInitialized = true;
}

