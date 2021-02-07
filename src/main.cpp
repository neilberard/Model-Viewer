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
//#include "scene.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include <windows.h>
#include <iostream>
#include "Camera.h"
#include "Texture.h"
#include "Render.h"
#include "Log.h"


#ifndef NOMINMAX
#define NOMINMAX
#endif
//SceneContext* scene;

namespace
{



	unsigned int uboBlock;


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
		unsigned int shadow_resolution = 1024;
		unsigned int SHADOW_WIDTH = shadow_resolution, SHADOW_HEIGHT = shadow_resolution;
		bool showLights = true;
		float shadowBias = 0.05;
		float scale = 0.05;
		glm::vec3 lightColorA = glm::vec3(1.0f);
		glm::mat4 lightScale = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
		glm::vec3 lightTranslate = glm::vec3(-5.f, 2.0f, .1f);
		// Shadows
		float far_dist = 20.0f;
		float near_dist = 0.0f;
	}

	namespace Shading
	{
		bool wireFrameOnShaded = false;
		bool drawReflections = true;
		bool drawShadows = true;
		bool drawTextures = false;
		bool drawNormals = true;
		bool drawSky = true;
		bool drawDebug = false;
		int mode = 0;
		glm::vec3 diffuseColor = glm::vec3(0.5f);
		float specIntensity = 1.0f;
		float specFalloff = 32.0f;
		Shader geoShader;

		// TEXTURE SLOTS
		int skyTextureSlot = 0;
		int shadowTextureSlot = 1;
		int depthTextureSlot = 2;
		int normalTextureSlot = 3;
		int diffuseTextureSlot = 4;


	}

	// DRAW MODE OPTIONS TODO: figure out how to convert this to an enum
	const char* items[]{ "Diffuse", "Normal", "Texture", "ZDepth", "Reflections", "UV" };

	GLuint SCREEN_WIDTH = 1080, SCREEN_HEIGHT = 720;
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;
	bool keys[1024];
	bool firstMouse = true;
	bool processMouse = false;
}


// ------------------------Uniform Block 

struct UniformBlock
	// Global Uniforms that can be accessed by all shaders
{
public:
	glm::mat4 mProj;
	glm::mat4 mView;
};



Model* assimpModel;
RenderContext* renderer;

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

void loadFBX(std::string fileNameStr = "")
{
	if (!fileNameStr.size())
	{
		fileNameStr = openfilename(pfilter).c_str();
		LOG_DEBUG("DONE LOADING ASSIMP");
	}
	assimpModel->LoadModel(fileNameStr.c_str());
}

void WindowSizeCallback(GLFWwindow *window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
};
void KeyCallback(GLFWwindow *window, int key, int scanecode, int action, int mode);
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoCameraMovement();

// Mouse movement callback
GLfloat lastX = SCREEN_WIDTH / 2.0f;
GLfloat lastY = SCREEN_WIDTH / 2.0f;


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
		//ImGui::SetWindowPos(ImVec2(0.9f, 1.0f), ImGuiCond_FirstUseEver);

		if (ImGui::Button("Load FBX"))
		{
			loadFBX();
		}
		if (ImGui::Button("Hot reload shaders", ImVec2(200, 30))) { ReloadShaders(); }
		
		
		// TODO: Swap out Shading with renderer.
		//ImGui::Checkbox("Wireframe on shaded", &renderer->wireFrameOnShaded);
		
		
		ImGui::Checkbox("Wireframe on shaded", &Shading::wireFrameOnShaded);

		ImGui::Checkbox("Show Lights", &Lights::showLights);
		ImGui::Checkbox("Draw Reflections", &Shading::drawReflections);
		ImGui::Checkbox("Draw Normals", &Shading::drawNormals);
		ImGui::Checkbox("Draw Shadows", &Shading::drawShadows);
		ImGui::Checkbox("Draw Sky", &Shading::drawSky);
		ImGui::Checkbox("Draw Debug", &Shading::drawDebug);
		ImGui::Checkbox("Draw Textures", &Shading::drawTextures);
		ImGui::Combo("Draw Mode", &Shading::mode, items, IM_ARRAYSIZE(items));
		ImGui::ColorEdit3("Diffuse Color", &Shading::diffuseColor.x);
		ImGui::SliderFloat("Spec Intensity", &Shading::specIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Spec Falloff", &Shading::specFalloff, 0.0f, 128.0f);
		ImGui::Separator();
		ImGui::Text("Turn Table");

		//ImGui::SliderFloat2("Move", &translate.x, -1.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::SliderFloat("Zoom", &scale, 0.0f, 4.0f);
		ImGui::SliderFloat3("Spin", &Scene::rotAxis.x, 0.0f, 1.0f);
		ImGui::SliderFloat("SpinSpeed", &Scene::spinSpeed, 0.0f, 0.05f);
		ImGui::SliderFloat("ZNear", &Lights::near_dist, 0.0f, 2.0f);
		ImGui::SliderFloat("ZFar", &Lights::far_dist, 0.001f, 20.0f);
		ImGui::Separator();
		ImGui::Text("Lights");
		ImGui::BeginGroup();
		ImGui::SliderFloat("Shadow Bias", &Lights::shadowBias, 0.0001f, 0.1f);
		ImGui::ColorEdit3("Light Color", &Lights::lightColorA.x);
		ImGui::SliderFloat3("LightPos", &Lights::lightTranslate.x, -5.0f, 5.0f);
		ImGui::EndGroup();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ProcessTransforms(float& a)
{
	GLfloat currentFrame = (GLfloat)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	Scene::proj = glm::perspective(Scene::camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 500.0f);
	Scene::view = Scene::camera.GetViewMatrix();
	a += Scene::spinSpeed;
	Scene::rotMat = glm::rotate(glm::mat4(1.0f), a, glm::normalize(Scene::rotAxis));
	Scene::model = glm::translate(glm::mat4(1.0f), Scene::translate);
	Scene::scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(Scene::scale));
	Scene::mvp = Scene::proj * Scene::view * Scene::model * Scene::scaleMatrix * Scene::rotMat;

	// Update View Projection
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, uboBlock));
	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, offsetof(UniformBlock, mProj), sizeof(glm::mat4), glm::value_ptr(Scene::proj)));
	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, offsetof(UniformBlock, mView), sizeof(glm::mat4), glm::value_ptr(Scene::view)));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

int main(void)
{
	Log::Init();

	//Log::set_level();
	LOG_INFO("Initialized Log!");


	/* Initialize the library */
	if (!glfwInit())
	{
		LOG_ERROR("GLFW failed to intialize!");
		return -1;
	}

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NB Model Viewer", NULL, NULL);
	if (!window)
	{
		LOG_INFO("Closing Window");
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);


	//--------------------- CALLBACKS ----------------------- //
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, WindowSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glewInit() != GLEW_OK)
	{
		LOG_CRITICAL("Glew failed to initialize!");

	}

	//---------------------Glew Initialized, Yay! -----------------------//

	LOG_DEBUG("Running OPENGL {}", glGetString(GL_VERSION));

	GLCall(glGenBuffers(1, &uboBlock));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, uboBlock));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), NULL, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	GLCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboBlock, 0, sizeof(UniformBlock)));


	// CREATE UBO BLOCK BEFORE INITIALIZING SHADERS!!
	assimpModel = new Model();
	SceneContext* sceneContext = new SceneContext;
	renderer = new RenderContext(sceneContext, window, assimpModel, &uboBlock);



	// ------------------- Scene Objects And Shaders ---------------------- //
	// Used for displaying textures such as the depthmap in debug mode, 
	SimplePlane renderPlane = SimplePlane(0.5f, false);
	// Test Texture 
	Texture diffuseMap("../../extern/resources/textures/Medieval_1K_diffuse.png");

	// Normal Map
	//Texture normalMap("../../extern/resources/textures/Medieval_1K_normal_flipped.png");
	Texture normalMap("../../extern/resources/textures/brickwall_normal.jpg");

	// Mesh Shader
	//Shader meshShader("../../resources/shaders/lambert.glsl", uboMatrices);
	//Scene::shaders.push_back(&meshShader);

	// BasicShader 
	Shader basicShader("../../resources/shaders/basic.glsl");
	Scene::shaders.push_back(&basicShader);

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
	Shader shadowShader("../../resources/shaders/shadows.glsl", uboBlock, "uBlock");
	Scene::shaders.push_back(&shadowShader);

	// normalmap shader 
	Shader normalShader("../../resources/shaders/normalMap.glsl", uboBlock, "uBlock");
	Scene::shaders.push_back(&normalShader);



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
	Skybox skyTexture(cubemap);
	Shader skyShader("../../resources/shaders/cubemap.glsl", uboBlock, "uBlock");
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

	// ------------------------------ FBO -----------------------------//



	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	// GL CONFIG
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	skyTexture.BindTexture(Shading::skyTextureSlot);
	//checkerMap.BindTexture(2);


	loadFBX("../../resources/fbx/box_scene2.fbx");

	//scene = new SceneContext((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);  // Planning to use this class to handle all the global variables.
	/* Loop until the user closes the window */
	float a = 0.0f;


	TransformNode myXform = TransformNode(&Scene::proj, &Scene::view);




	while (!glfwWindowShouldClose(window))
	{

		ProcessTransforms(a);
		DoCameraMovement();

		renderer->onDisplay();

		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT, GL_FILL);

		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, Lights::near_dist, Lights::far_dist);
		glm::mat4 lightView = glm::lookAt(Lights::lightTranslate, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		if (Shading::drawShadows)
		{
			renderer->mLightSpace = lightSpaceMatrix;
			renderer->mModelSpace = Scene::model * Scene::rotMat;
			renderer->renderShadows();

			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		}
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		renderer->mDepthFBO.BindTexture(Shading::depthTextureSlot);
		diffuseMap.BindTexture(Shading::diffuseTextureSlot);
		normalMap.BindTexture(Shading::normalTextureSlot);



		// RENDER SHADOW SHADER
		shadowShader.Bind();

		shadowShader.SetUniform1i("uNormalMap", Shading::normalTextureSlot);
		shadowShader.SetUniform1i("uDiffuseMap", Shading::diffuseTextureSlot);
		shadowShader.SetUniform1i("uShadowMap", Shading::depthTextureSlot);
		shadowShader.SetUniform1i("uSky", Shading::skyTextureSlot);


		shadowShader.SetUniform1f("uShadowBias", Lights::shadowBias);
		shadowShader.SetUniform1i("uDrawMode", Shading::mode);
		shadowShader.SetUniform1i("uWireframe", 0);

		shadowShader.SetUniform1f("uSpecular", Shading::specIntensity);
		shadowShader.SetUniform1f("uGlossiness", Shading::specFalloff);

		shadowShader.SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
		shadowShader.SetUniformMat4f("uModel", Scene::model * Scene::rotMat);
		shadowShader.SetUniform3f("uColor", Shading::diffuseColor);
		shadowShader.SetUniform3f("uViewPos", Scene::camera.Position());
		shadowShader.SetUniform3f("lightPos", Lights::lightTranslate);
		// Bool
		shadowShader.SetUniform1i("uDrawTexture", Shading::drawTextures);
		shadowShader.SetUniform1i("uDrawNormal", Shading::drawNormals);
		shadowShader.SetUniform1i("uDrawReflection", Shading::drawReflections);
		shadowShader.SetUniform1i("uDrawShadow", Shading::drawShadows);
		renderer->onDisplay();


		if (Shading::wireFrameOnShaded)
		{
			glPolygonMode(GL_FRONT, GL_LINE);
			shadowShader.SetUniform1i("uWireframe", Shading::wireFrameOnShaded);
			glLineWidth(1.0);
			glCullFace(GL_BACK);
			renderer->onDisplay();
		}


		shadowShader.UnBind();

		//// ~RENDER SHADOW SHADER
		renderer->mDepthFBO.UnbindTexture();



		// Second pass
		if (Shading::drawSky)
		{
			renderer->renderSky();
		}

		if (Lights::showLights)
		{

			glm::vec4 lightPos = Scene::proj * Scene::view * glm::vec4(Lights::lightTranslate.x, Lights::lightTranslate.y, Lights::lightTranslate.z, 1.0);
			glm::vec4 mapCenter = Scene::mvp * glm::vec4(0.0, 0.0, 0.0, 1.0);
			glColor3f(1.0, 0.0, 0.0);
			glLineWidth(2.0);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			// Draw light angle line
			glBegin(GL_LINES);
			glVertex4f(mapCenter.x, mapCenter.y, mapCenter.z, mapCenter.w);
			glVertex4f(lightPos.x, lightPos.y, lightPos.z, lightPos.w);
			glEnd();


			lightShader.Bind();
			glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), Lights::lightTranslate);

			lightShader.SetUniform3f("u_LightColorA", Lights::lightColorA);
			lightShader.SetUniformMat4f("uModel", lightModel * Lights::lightScale);
			glCullFace(GL_FRONT);
			glDepthMask(GL_TRUE);
			lightCube.draw();
			lightShader.UnBind();

			//myXform.setPosition(Lights::lightTranslate);
			myXform.setModel(lightModel);
			myXform.draw();
		}


		if (Shading::drawDebug)
		{
			postShader.Bind();
			glDisable(GL_DEPTH_TEST);
			glCullFace(GL_BACK);
			renderer->mDepthFBO.BindTexture(2);
			postShader.SetUniform1f("u_Near", Lights::near_dist);
			postShader.SetUniform1f("u_Far", Lights::far_dist);
			postShader.SetUniform1i("u_Texture", 2);
			//postShader.SetUniformMat4f("u_MVP", Scene::model);
			renderPlane.Draw();
			postShader.UnBind();
			renderer->mDepthFBO.UnbindTexture();
		}


		ProcessUI();
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	LOG_INFO("Shutting down");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	delete renderer;
	delete sceneContext;
	delete assimpModel;

	return 0;
}

void DoCameraMovement()
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
		//assimpModel = Model(fileNameStr.c_str());
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
