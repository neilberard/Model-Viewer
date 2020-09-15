#pragma  once
#include "Log.h"
#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include <gl/glew.h>

//namespace
//{
//
//
//
//
//
//
//	void WindowSizeCallback(GLFWwindow *window, int width, int height)
//	{
//		SCREEN_WIDTH = width;
//		SCREEN_HEIGHT = height;
//	};
//
//	void DoCameraMovement()
//	{
//		if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
//		{
//			Scene::camera.ProcessKeyboard(FORWARD, deltaTime);
//		}
//
//		if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
//		{
//			Scene::camera.ProcessKeyboard(BACKWARD, deltaTime);
//		}
//
//		if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
//		{
//			Scene::camera.ProcessKeyboard(LEFT, deltaTime);
//		}
//
//		if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
//		{
//			Scene::camera.ProcessKeyboard(RIGHT, deltaTime);
//		}
//	}
//
//	void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
//	{
//		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		{
//			glfwSetWindowShouldClose(window, GL_TRUE);
//		}
//
//		// PROCESS MOUSE
//		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
//		{
//			if (processMouse)
//			{
//				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//				processMouse = false;
//			}
//			else
//			{
//				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//				processMouse = true;
//			}
//		}
//
//
//		if (key >= 0 && key < 1024)
//		{
//			if (GLFW_PRESS == action)
//			{
//				keys[key] = true;
//			}
//			else if (GLFW_RELEASE == action)
//			{
//				keys[key] = false;
//			}
//
//		}
//
//	}
//
//	void MouseCallback(GLFWwindow *window, double pXPos, double pYPos)
//	{
//		if (!processMouse)
//		{
//			return;
//		}
//
//		GLfloat xPos = GLfloat(pXPos);
//		GLfloat yPos = GLfloat(pYPos);
//
//
//		if (firstMouse)
//		{
//			lastX = xPos;
//			lastY = yPos;
//			firstMouse = false;
//		}
//
//
//		GLfloat xOffset = xPos - lastX;
//		GLfloat yOffset = yPos - lastY;
//
//		lastX = xPos;
//		lastY = yPos;
//
//		Scene::camera.ProcessMouseMovement(xOffset, yOffset * -1.0f);
//
//	}
//
//	void ScrollCallback(GLFWwindow *window, double xOffset, double pYOffset)
//	{
//		GLfloat yOffset = GLfloat(pYOffset);
//		Scene::camera.ProcessMouseScroll(yOffset);
//	}
//
//}



class SceneContext
{
public:

	SceneContext(int pScreenWidth, int pScreenHeight);
	GLFWwindow *mWindow;

	bool drawLights;
	bool drawGeo;
	

	bool OnDisplay();
	void Init();


private:
	bool mInitialized = false;
	int mScreenWidth, mScreenHeight;
	std::vector<Shader*>shaders;


};

