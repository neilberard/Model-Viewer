// Author: Neil Berard. External code and licenses included in the project directory and git submodules.
// This project is intended for learning opengl and not for any real-world applications. Use at your own risk. 


///////////////////// TEST CODE ///////////////////////

//#include "Model.h"
//#include <iostream>
//
//int main()
//{
//	printf("Howdy!");
//	std::cin.get();
//	return 1;
//}

///////////////////// END TEST CODE ///////////////////////


#include "Debugging.h"
#include <iostream>
#include <memory>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "Debugging.h"
#include "Model.h"
#include "Shader.h"
#include "scene.h"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include <windows.h>
#include <iostream>
#include "Camera.h"
#include "Texture.h"
#include "Log.h"


#ifndef NOMINMAX
#define NOMINMAX
#endif


SceneContext* scene;

// UI STATE
namespace Scene
{
	//TODO: Add active camera index.
	Camera camera(glm::vec3(0.0f, 0.5f, 3.0f));
	SimpleCube* sky = NULL;
	SimpleCube* lightCube = NULL;
	
	glm::mat4 proj = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 viewRot = glm::mat4(1.0f); // Just the rotation matrix
	glm::mat4 mvp = glm::mat4(1.0f);
	std::vector<Shader*>shaders;

	std::string cubeMapDir("../../extern/resources/textures/skyA/");
	//std::string cubeMapDir("../../resources/textures/cubemaps/crater_lake/");
	std::string cubeMapFormat("jpg");
	// Turn Table
	float spinSpeed = 0.000;
	float scale = 1.0f;
	glm::vec3 rotAxis(0.0, 1.0, 0.0);
	glm::mat4 rotMat;
	glm::vec3 translate(0.0);

}

namespace Lights
{
	unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	bool showLights = true;
	float scale = 0.05;
	glm::vec3 lightColorA = glm::vec3(1.0f);
	glm::mat4 lightScale = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	glm::vec3 lightTranslate = glm::vec3(0.0f, 2.0f, .1f);	
	// Shadows
	float far_dist = 5.0f;
	float near_dist = 0.0f;
}

namespace Shading
{
	bool wireFrameOnShaded = false;
	bool drawReflections = true;
	bool drawShadows = true;
	bool drawTextures = true;
	bool drawSky = true;
	bool drawDebug = false;
	int mode = 0;
	glm::vec3 diffuseColor = glm::vec3(0.5f);
	float specIntensity = 1.0f;
	float specFalloff = 32.0f;
	Shader geoShader;
}

// DRAW MODE OPTIONS TODO: figure out how to convert this to an enum
const char* items[]{ "Diffuse", "Normal", "Texture", "ZDepth", "Reflections", "UV" };


const GLuint WIDTH = 1080, HEIGHT = 720;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool keys[1024];
bool firstMouse = true;
bool processMouse = false;

Model assimpModel;



// Returns an empty string if dialog is canceled
const char* pfilter = "All Files (*.*)\0*.*\0";

std::string openfilename(const char *filter) {
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = (char*)filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	std::string fileNameStr;
	if (GetOpenFileName(&ofn))
	{
		fileNameStr = fileName;
	}
	return fileNameStr;
}

std::string getexepath()
{
	char result[MAX_PATH];
	return std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
}

void loadFBX(std::string fileNameStr="")
{
	if (!fileNameStr.size())
	{
		fileNameStr = openfilename(pfilter).c_str();
		LOG_DEBUG("DONE LOADING ASSIMP");
	}
	assimpModel = Model(fileNameStr.c_str());
}

void ScreenResize(int width, int height, glm::mat4 &proj)
{
	float ratio = float(width) / float(height);

	proj = glm::ortho(-1.0f * ratio, // Left 
		1.0f * ratio, // right
		-1.0f, // bottom 
		1.0f, // Top
		-5.0f, // near 
		5.0f); // far
}


void KeyCallback(GLFWwindow *window, int key, int scanecode, int action, int mode);
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();


GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = WIDTH / 2.0f;


void ReloadShaders()
{
	LOG_INFO("Reloading Shaders");
	for each (Shader* shader in Scene::shaders)
	{
		shader->Reload();
	}
}

void ProcessUI()
{

	/////////////////  IMGUI  /////////////////

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{

		ImGui::Begin(" ");                          // Create a window called "Hello, world!" and append into it.
		if (ImGui::Button("Load FBX"))
		{
			loadFBX();
		}
		if (ImGui::Button("Hot reload shaders", ImVec2(200, 50))) { ReloadShaders(); }
		ImGui::Checkbox("Wireframe on shaded", &Shading::wireFrameOnShaded);
		ImGui::Checkbox("Show Lights", &Lights::showLights);
		ImGui::Checkbox("Draw Reflections", &Shading::drawReflections);
		ImGui::Checkbox("Draw Shadows", &Shading::drawShadows);
		ImGui::Checkbox("Draw Sky", &Shading::drawSky);
		ImGui::Checkbox("Draw Debug", &Shading::drawDebug);
		ImGui::Checkbox("Draw Textures", &Shading::drawTextures);
		ImGui::Combo("Draw Mode", &Shading::mode, items, IM_ARRAYSIZE(items));
		ImGui::ColorEdit3("Diffuse Color", &Shading::diffuseColor.x);
		ImGui::SliderFloat("Spec Intensity", &Shading::specIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Spec Falloff", &Shading::specFalloff, 0.0f, 128.0f);

		//ImGui::SliderFloat2("Move", &translate.x, -1.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::SliderFloat("Zoom", &scale, 0.0f, 4.0f);
		ImGui::SliderFloat3("Spin", &Scene::rotAxis.x, 0.0f, 1.0f);
		ImGui::SliderFloat("SpinSpeed", &Scene::spinSpeed, 0.0f, 0.05f);
		ImGui::SliderFloat("ZNear", &Lights::near_dist, 0.0f, 2.0f);
		ImGui::SliderFloat("ZFar", &Lights::far_dist, 0.001f, 10.0f);
		ImGui::Separator();

		ImGui::ColorEdit3("Light Color", &Lights::lightColorA.x);
		ImGui::SliderFloat3("LightPos", &Lights::lightTranslate.x, -2.0f, 2.0f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RenderGeo(Shader& geoShader)
{

	// DRAW SHADER //
	geoShader.Bind();
	geoShader.SetUniform1i("u_Sky", 0);
	geoShader.SetUniform1i("u_DrawTextures", Shading::drawTextures);
	geoShader.SetUniform1i("u_DrawReflections", Shading::drawReflections);
	geoShader.SetUniform1i("u_DrawMode", Shading::mode);
	geoShader.SetUniform1i("u_Wireframe", 0);
	geoShader.SetUniform3f("u_LightPosA", Lights::lightTranslate);
	geoShader.SetUniform3f("u_LightColorA", Lights::lightColorA);
	geoShader.SetUniform3f("u_Color", Shading::diffuseColor);
	geoShader.SetUniform3f("u_CameraPos", Scene::camera.Position());
	geoShader.SetUniform1f("u_SpecIntensity", Shading::specIntensity);
	geoShader.SetUniform1f("u_SpecFalloff", Shading::specFalloff);
	geoShader.SetUniform1i("u_Texture", 1);
	geoShader.SetUniformMat4f("u_MVP", Scene::mvp);
	geoShader.SetUniformMat4f("u_ModelMatrix", Scene::rotMat);
	glPolygonMode(GL_FRONT, GL_FILL);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);

	assimpModel.Draw();

	// Wireframe on shaded.
	// Draw a second pass.
	if (Shading::wireFrameOnShaded)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		geoShader.SetUniform1i("u_Wireframe", Shading::wireFrameOnShaded);
		geoShader.SetUniform3f("u_Color", 1.0f, 1.0f, 1.0f);
		assimpModel.Draw();
	}
	Shading::geoShader.UnBind();
}

void RenderLights(Shader& lightShader)
{
	glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), Lights::lightTranslate);
	//glCullFace(GL_BACK);
	lightShader.Bind();
	lightShader.SetUniform3f("u_LightPosA", Lights::lightTranslate);
	lightShader.SetUniform3f("u_LightColorA", Lights::lightColorA);
	lightShader.SetUniformMat4f("u_MVP", Scene::proj * Scene::view * lightModel * Lights::lightScale);
	glPolygonMode(GL_FRONT, GL_FILL);
	if (Scene::lightCube) 
	{ 
		Scene::lightCube->draw(); 
	}
	else
	{
		LOG_WARNING("Light Cube not initialized!");
	}

	lightShader.UnBind();

}

void ProcessTransforms(float& a)
{
	GLfloat currentFrame = (GLfloat)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	Scene::proj = glm::perspective(Scene::camera.GetZoom(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 500.0f);
	Scene::view = Scene::camera.GetViewMatrix();
	a += Scene::spinSpeed;
	Scene::rotMat = glm::rotate(glm::mat4(1.0f), a, glm::normalize(Scene::rotAxis));
	Scene::model = glm::translate(glm::mat4(1.0f), Scene::translate);
	Scene::scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(Scene::scale));
	Scene::mvp = Scene::proj * Scene::view * Scene::model * Scene::scaleMatrix * Scene::rotMat;
}

int main(void)
{
	Log::Init();

	//Log::set_level();
	LOG_INFO("Initialized Log!");
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
	{
		LOG_ERROR("GLFW failed to intialize!");
		return -1;
	}

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "NB Model Viewer", NULL, NULL);
	if (!window)
	{
		LOG_INFO("Closing Window");
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glewInit() != GLEW_OK)
	{
		LOG_CRITICAL("Glew failed to initialize!");

	}
	LOG_DEBUG("Running OPENGL {}", glGetString(GL_VERSION));

	SimplePlane renderPlane = SimplePlane(0.5f, false);

	// Test Texture 
	Texture checkerMap("../../extern/resources/textures/checker1024.png");

	// Mesh Shader
	Shader meshShader("../../resources/shaders/lambert.glsl");
	Scene::shaders.push_back(&meshShader);

	// Post-process 
	Shader postShader("../../resources/shaders/post.glsl");
	Scene::shaders.push_back(&postShader);

	// Shadow Pass
	Shader simpleDepthShader("../../resources/shaders/simpleDepthShader.glsl");
	Scene::shaders.push_back(&simpleDepthShader);

	// light Cube
	SimpleCube lightCube = SimpleCube(1.0f, false);

	Shader lightShader("../../resources/shaders/light.glsl");
	Scene::shaders.push_back(&lightShader);

	// shadow shader 
	Shader shadowShader("../../resources/shaders/shadows.glsl");
	Scene::shaders.push_back(&shadowShader);




	// Skybox
	std::vector<std::string>cubemap
	{
	Scene::cubeMapDir + "right" + "." + Scene::cubeMapFormat,
	Scene::cubeMapDir + "left" + "." + Scene::cubeMapFormat,
	Scene::cubeMapDir + "top" + "." + Scene::cubeMapFormat,
	Scene::cubeMapDir + "bottom" + "." + Scene::cubeMapFormat,
	Scene::cubeMapDir + "front" + "." + Scene::cubeMapFormat,
	Scene::cubeMapDir + "back" + "." + Scene::cubeMapFormat
	};
	Skybox skyMap(cubemap);
	Shader skyShader("../../resources/shaders/cubemap.glsl");
	Scene::shaders.push_back(&skyShader);
	SimpleCube skyCube(1.0, false);

	// GET EXE DIRECTORY
	std::string exePath = getexepath();
	LOG_DEBUG("\n EXE PATH:    {}", exePath.c_str());


	// Setup Dear ImGui context
	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// GL CONFIG
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	// Frame buffer
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Attach Texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Lights::SHADOW_WIDTH, Lights::SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_DEBUG("Frame buffer Fbo complete!");
	}
	else
	{
		LOG_ERROR("No Frame buffer!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ~Frame buffer

	skyMap.BindTexture(0);
	checkerMap.BindTexture(1);
	loadFBX("../../resources/fbx/box_scene2.fbx");

	scene = new SceneContext((int)WIDTH, (int)HEIGHT);  // Planning to use this class to handle all the global variables.
	/* Loop until the user closes the window */
	float a = 0.0f;


	while (!glfwWindowShouldClose(window))
	{

		ProcessTransforms(a);
		DoMovement();
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT, GL_FILL);

		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, Lights::near_dist, Lights::far_dist);
		glm::mat4 lightView = glm::lookAt(Lights::lightTranslate, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView * Scene::model * Scene::scaleMatrix;

		if (Shading::drawShadows)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glViewport(0, 0, Lights::SHADOW_WIDTH, Lights::SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_DEPTH_BUFFER_BIT);
			simpleDepthShader.Bind();
			simpleDepthShader.SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix * Scene::rotMat);
			simpleDepthShader.SetUniformMat4f("u_Model", Scene::model);
			assimpModel.Draw();
			simpleDepthShader.UnBind();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, WIDTH, HEIGHT);
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		}
		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);


		// RENDER SHADOW SHADER
		shadowShader.Bind();
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, depthMap);


		shadowShader.SetUniform1i("u_ShadowMap", 2);
		shadowShader.SetUniform1i("u_Texture", 1);
		shadowShader.SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
		shadowShader.SetUniformMat4f("u_Projection", Scene::proj);
		shadowShader.SetUniformMat4f("u_View", Scene::view);
		shadowShader.SetUniformMat4f("u_Model", Scene::model * Scene::rotMat);
		shadowShader.SetUniform3f("viewPos", Scene::camera.Position());
		shadowShader.SetUniform3f("lightPos", Lights::lightTranslate);

		assimpModel.Draw();


		//RenderGeo(meshShader);


		// Second pass
		if (Shading::drawSky)
		{
			glCullFace(GL_FRONT);
			glDepthMask(GL_FALSE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			skyShader.Bind();
			Scene::viewRot = glm::mat3(Scene::view);
			skyShader.SetUniformMat4f("u_MVP", Scene::proj * Scene::viewRot);
			skyShader.SetUniform1i("u_Sky", 0);
			skyCube.draw();
			skyShader.UnBind();
			glCullFace(GL_BACK);
			glDepthMask(GL_TRUE);
		}

		if (Lights::showLights)
		{
			glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), Lights::lightTranslate);
			lightShader.Bind();
			lightShader.SetUniform3f("u_LightPosA", Lights::lightTranslate);
			lightShader.SetUniform3f("u_LightColorA", Lights::lightColorA);
			lightShader.SetUniformMat4f("u_MVP", Scene::proj * Scene::view * lightModel * Lights::lightScale);
			glCullFace(GL_FRONT);
			glDepthMask(GL_TRUE);
			lightCube.draw();
		}


		if (Shading::drawDebug)
		{
			postShader.Bind();
			glDisable(GL_DEPTH_TEST);
			glCullFace(GL_BACK);
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			postShader.SetUniform1f("u_Near", Lights::near_dist);
			postShader.SetUniform1f("u_Far", Lights::far_dist);

			postShader.SetUniform1i("u_Texture", 2);
			postShader.SetUniformMat4f("u_MVP", Scene::model);
			renderPlane.Draw();
			postShader.UnBind();
		}
		

		ProcessUI();
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	LOG_INFO("Shutting down");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	delete scene;
	return 0;
}

void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		Scene::camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		Scene::camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		Scene::camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		Scene::camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		std::string fileNameStr = openfilename(pfilter).c_str();
		assimpModel = Model(fileNameStr.c_str());
	}

	// PROCESS MOUSE
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (processMouse)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			processMouse = false;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			processMouse = true;
		}
	}


	if (key >= 0 && key < 1024)
	{
		if (GLFW_PRESS == action)
		{
			keys[key] = true;
		}
		else if (GLFW_RELEASE == action)
		{
			keys[key] = false;
		}

	}

}

void MouseCallback(GLFWwindow *window, double pXPos, double pYPos)
{
	if (!processMouse)
	{
		return;
	}

	GLfloat xPos = GLfloat(pXPos);
	GLfloat yPos = GLfloat(pYPos);


	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}


	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = yPos - lastY;

	lastX = xPos;
	lastY = yPos;

	Scene::camera.ProcessMouseMovement(xOffset, yOffset * -1.0f);

}

void ScrollCallback(GLFWwindow *window, double xOffset, double pYOffset)
{
	GLfloat yOffset = GLfloat(pYOffset);
	Scene::camera.ProcessMouseScroll(yOffset);
}
