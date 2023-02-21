// Author: Neil Berard. External code and licenses included in the project directory and git submodules.
// This project is intended for learning opengl and not for any real-world applications. Use at your own risk. 

#define TEST_CODE


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

#ifdef TEST_CODE
#include "stb/stb_image.h"

void renderCube();
#endif // TEST_CODE

namespace
{

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
		// lights
		glm::vec3 lightPositions[] = {
			glm::vec3(-10.0f,  10.0f, 0.0f),
			glm::vec3(10.0f,  10.0f, 10.0f),
			glm::vec3(-10.0f, -10.0f, 10.0f),
			glm::vec3(10.0f, -10.0f, 10.0f),
		};
		glm::vec3 lightColors[] = {
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f)
		};
		float shadowStrength = 1.0;

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

		float metallic = 0.0;
		float roughness = 0.2;

		Shader geoShader;

		// TEXTURE SLOTS
		int skyTextureSlot = 0;
		int shadowTextureSlot = 1;
		int depthTextureSlot = 2;
		int normalTextureSlot = 3;
		int diffuseTextureSlot = 4;


	}

	// DRAW MODE OPTIONS TODO: figure out how to convert this to an enum
	const char* items[]{ "Diffuse", "Normal", "Texture", "ZDepth", "Reflections", "UV", "NormalMap"};

	GLuint SCREEN_WIDTH = 1080, SCREEN_HEIGHT = 720;
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;
	bool keys[1024];
	bool firstMouse = true;
	bool processMouse = false;
}


Model* assimpModel = nullptr;
SceneContext* sceneContext = new SceneContext();
RenderContext* renderContext = nullptr;


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
	if (!fileNameStr.size())  // Open File dialog if no filename is provided.
	{
		fileNameStr = openfilename(pfilter).c_str();
	}
	assimpModel->LoadModel(fileNameStr.c_str());
}


void loadIBL(std::string fileNameStr = "")
{
	if (!fileNameStr.size())  // Open File dialog if no filename is provided.
	{
		fileNameStr = openfilename(pfilter).c_str();
	}
	if (fileNameStr.size())
	{
		renderContext->loadIBL(fileNameStr.c_str());
	}

}


void WindowSizeCallback(GLFWwindow *window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	if (renderContext != nullptr) { renderContext->resize(); }

};
void KeyCallback(GLFWwindow *window, int key, int scanecode, int action, int mode);
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void MousePressedCallback(GLFWwindow *window, int button, int action, int mods);


void DoCameraMovement();

// Mouse movement callback
GLfloat lastX = SCREEN_WIDTH / 2.0f;
GLfloat lastY = SCREEN_WIDTH / 2.0f;


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


		// Load IBL
		if (ImGui::Button("Load IBL"))
		{
			loadIBL();
		}

		if (ImGui::Button("Reload shaders", ImVec2(200, 30))) { renderContext->reloadShaders(); }


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
		ImGui::Separator();
		ImGui::Text("Turn Table");

		//ImGui::SliderFloat2("Move", &translate.x, -1.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::SliderFloat("Zoom", &scale, 0.0f, 4.0f);
		//ImGui::SliderFloat3("Spin", &Scene::rotAxis.x, 0.0f, 1.0f);
		ImGui::SliderFloat("SpinSpeed", &Scene::spinSpeed, 0.0f, 0.05f);
		ImGui::SliderFloat("ZNear", &Lights::near_dist, 0.0f, 2.0f);
		ImGui::SliderFloat("ZFar", &Lights::far_dist, 0.001f, 20.0f);
		ImGui::Separator();
		ImGui::Text("Lights");
		ImGui::BeginGroup();
		ImGui::SliderFloat("Shadow Bias", &Lights::shadowBias, 0.0001f, 0.1f);
		ImGui::SliderFloat("Shadow Strength", &Lights::shadowStrength, 0.0f, 1.0f);
		ImGui::ColorEdit3("Light Color", &Lights::lightColors[0].x);
		ImGui::SliderFloat3("LightPos", &Lights::lightTranslate.x, -5.0f, 5.0f);
		ImGui::EndGroup();
		ImGui::BeginGroup();
		ImGui::Text("Material");
		ImGui::SliderFloat("Metallic", &Shading::metallic, 0.0001f, 1.0f);
		ImGui::SliderFloat("Roughness", &Shading::roughness, 0.0001f, 1.0f);
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
	Scene::model = glm::translate(glm::mat4(1.0f), Scene::translate) * Scene::scaleMatrix * Scene::rotMat;
	Scene::scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(Scene::scale));
	Scene::mvp = Scene::proj * Scene::view * Scene::model;// *Scene::scaleMatrix * Scene::rotMat;
}


int main(void)
{
	Log::Init();
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
	glfwSetMouseButtonCallback(window, MousePressedCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glewInit() != GLEW_OK)
	{
		LOG_CRITICAL("Glew failed to initialize!");

	}

	//---------------------Glew Initialized, Yay! -----------------------//

	LOG_DEBUG("Running OPENGL {}", glGetString(GL_VERSION));
	assimpModel = new Model();


	// ------------------- Scene Objects And Shaders ---------------------- //
	// Used for displaying textures such as the depth map in debug mode, 
	SimplePlane renderPlane = SimplePlane(0.5f, false);

	// light Cube
	SimpleCube lightCube = SimpleCube(1.0f, false);

	Shader lightShader = Shader("../../resources/shaders/light.glsl");
	sceneContext->addShader(lightShader);

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


	loadFBX("../../resources/fbx/box_scene2.fbx");


	float a = 0.0f;
	TransformNode myXform = TransformNode(&Scene::proj, &Scene::view);


	// -------------------------------- RENDER CONTEXT ----------------------------------
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	
#ifndef TEST_CODE

	renderContext = new RenderContext(sceneContext, window, assimpModel);
	renderContext->loadIBL("../../resources/textures/IBL/fin4_Ref.hdr");
	renderContext->mDrawDebug = RenderContext::NORMAL;

#endif // TEST_CODE
	// Active
#ifdef TEST_CODE
	Shader equirectangularToCubemapShader("../../resources/shaders/equirectangularMap.glsl");
	Shader backgroundShader("../../resources/shaders/cubemap.glsl");


	backgroundShader.bindShader();
	backgroundShader.SetUniform1i("environmentMap", 0);
	backgroundShader.SetUniformMat4f("projection", Scene::proj);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.


	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// pbr: load the HDR environment map
	// ---------------------------------
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf("../../resources/textures/IBL/Arches_E_PineTree_3k.hdr", &width, &height, &nrComponents, 0);
	unsigned int hdrTexture = 0;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}

	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	equirectangularToCubemapShader.bindShader();
	equirectangularToCubemapShader.SetUniform1i("equirectangularMap", 0);
	equirectangularToCubemapShader.SetUniformMat4f("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.SetUniformMat4f("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	backgroundShader.bindShader();
	backgroundShader.SetUniformMat4f("view", Scene::view);


#endif // TEST_CODE

	while (!glfwWindowShouldClose(window))
	{
		ProcessTransforms(a);
		DoCameraMovement();

		//glEnable(GL_DEPTH_TEST);
		//glPolygonMode(GL_FRONT, GL_FILL);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);



		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, Lights::near_dist, Lights::far_dist);
		glm::mat4 lightView = glm::lookAt(Lights::lightTranslate, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;


#ifdef TEST_CODE

		
		//equirectangularToCubemapShader.bindShader();
		//equirectangularToCubemapShader.SetUniformMat4f("view", Scene::view);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, hdrTexture);
		//renderCube();




		backgroundShader.SetUniform1i("environmentMap", envCubemap);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		renderCube();


		ProcessUI();
		glfwSwapBuffers(window);
		glfwPollEvents();
		continue;
#else
		if (Shading::drawShadows)
		{
			renderContext->mLightSpace = lightSpaceMatrix;
			renderContext->mModelSpace = Scene::model;
			renderContext->renderShadows();

			//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		}
		
		
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// RENDER Diffuse SHADER
				// bind pre-computed IBL data


		renderContext->mIrradianceCubeMap->bindTexture(0);
		renderContext->mPrefilterCubeMap->bindTexture(1);
		renderContext->mBrdfLUTTexture->bindTexture(2);

		renderContext->mNormalMap->bindTexture(3);

		renderContext->mPbrShader->bindShader();
		renderContext->mPbrShader->SetUniform1i("uShadowMap", renderContext->mDepthFBO->mTexture);
		renderContext->mPbrShader->SetUniformMat4f("model", Scene::model);
		renderContext->mPbrShader->SetUniformMat4f("projection", Scene::proj);
		renderContext->mPbrShader->SetUniformMat4f("view", Scene::view);
		renderContext->mPbrShader->SetUniform3f("camPos", Scene::camera.Position());
		
		renderContext->mPbrShader->SetUniform1i("irradianceMap", renderContext->mIrradianceCubeMap->getSlot());
		renderContext->mPbrShader->SetUniform1i("prefilterMap", renderContext->mPrefilterCubeMap->getSlot());
		renderContext->mPbrShader->SetUniform1i("brdfLUT", renderContext->mBrdfLUTTexture->getSlot());
		renderContext->mPbrShader->SetUniform3f("albedo", Shading::diffuseColor);
		renderContext->mPbrShader->SetUniform1f("ao", 1.0f);
		renderContext->mPbrShader->SetUniform1f("metallic", Shading::metallic);
		renderContext->mPbrShader->SetUniform1f("roughness", Shading::roughness);
		renderContext->mPbrShader->SetUniform1f("uShadowStrength", Lights::shadowStrength);
		renderContext->mPbrShader->SetUniform1i("uNormalMap", renderContext->mNormalMap->getSlot());

		// OPTIONS
		renderContext->mPbrShader->SetUniform1i("uDrawNormal", Shading::drawNormals);
		renderContext->mPbrShader->SetUniform1i("uDrawShadow", Shading::drawShadows);



		for (unsigned int i = 0; i < sizeof(Lights::lightPositions) / sizeof(Lights::lightPositions[0]); ++i)
		{
			glm::vec3 newPos = Lights::lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);

			if (i == 0)
			{
				newPos = Lights::lightTranslate;
			}
			else
			{
				newPos = Lights::lightPositions[i];
			}

			renderContext->mPbrShader->SetUniform3f("lightPositions[" + std::to_string(i) + "]", newPos);
			renderContext->mPbrShader->SetUniform3f("lightColors[" + std::to_string(i) + "]", Lights::lightColors[i]);
		}

		renderContext->onDisplay();

		renderContext->mPbrShader->unbindShader();



		// ~RENDER SHADOW SHADER
		//renderContext->mDepthFBO->unbindTexture();


		// Second pass
		if (Shading::drawSky)
		{
			glCullFace(GL_FRONT);
			glDepthMask(GL_FALSE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			renderContext->mEnvCubeMap->bindTexture(4);
			renderContext->mSkyShader->bindShader();
			renderContext->mSkyShader->SetUniform1i("uSky", renderContext->mEnvCubeMap->getSlot());
			renderContext->renderCube();

			renderContext->mEnvCubeMap->unbindTexture();
			renderContext->mSkyShader->unbindShader();

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


			lightShader.bindShader();
			glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), Lights::lightTranslate);

			lightShader.SetUniform3f("u_LightColorA", Lights::lightColorA);
			lightShader.SetUniformMat4f("uModel", lightModel * Lights::lightScale);
			glCullFace(GL_FRONT);
			glDepthMask(GL_TRUE);
			lightCube.Draw();
			lightShader.unbindShader();

			//myXform.setPosition(Lights::lightTranslate);
			myXform.setModel(lightModel);
			myXform.draw();
		}

		//brdfShader->Bind();
		//renderQuad();


		if (Shading::drawDebug)
		{
			renderContext->mPostShader->bindShader();
			renderContext->mDepthFBO->bindTexture();
			glDisable(GL_DEPTH_TEST);
			glCullFace(GL_BACK);
			renderContext->mPostShader->SetUniform1f("u_Near", Lights::near_dist);
			renderContext->mPostShader->SetUniform1f("u_Far", Lights::far_dist);
			renderContext->mPostShader->SetUniform1i("u_Texture", renderContext->mDepthFBO->mTexture);
			renderPlane.Draw();
			renderContext->mPostShader->unbindShader();
			renderContext->mDepthFBO->unbindTexture();
		}


		ProcessUI();
		glfwSwapBuffers(window);
		glfwPollEvents();
#endif // TEST_CODE

	}

	LOG_INFO("Shutting down");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	delete renderContext;
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


void MousePressedCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (renderContext != nullptr)
		{
			renderContext->selectObject(xpos, ypos);
		}
	}
}

#ifdef TEST_CODE
// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


#endif // TEST_CODE

