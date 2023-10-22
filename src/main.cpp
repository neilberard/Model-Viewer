#include <iostream>
#include <windows.h>
#include <shobjidl_core.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

#include "Render.h"
#include "Log.h"
#include "Shader.h"
#include "Camera.h"

#define DEPTH_PASS

// Forward Declarations
void loadFBX(std::string fileNameStr = "");
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderSphere();
void renderCube();
void ProcessUI();

Model* assimpModel = nullptr;


// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

RenderContext* renderer;

//
bool processMouse = true;


float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	Log::Init();
	LOG_INFO("Initialized Log!");


	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	if (glewInit() != GLEW_OK)
	{
		LOG_CRITICAL("Glew failed to initialize!");

	}
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



	// lights
// ------
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, 10.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;


	renderer = new RenderContext(window);

	// initialize static shader uniforms before rendering
	// --------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	renderer->mPbrShader->bindShader();
	renderer->mPbrShader->SetUniformMat4f("projection", projection);
	renderer->mPbrShader->SetUniform1i("irradianceMap", 0);
	renderer->mPbrShader->SetUniform1i("prefilterMap", 1);
	renderer->mPbrShader->SetUniform1i("brdfLUT", 2);
	renderer->mPbrShader->SetUniform3f("albedo", 0.5f, 0.0f, 0.0f);
	renderer->mPbrShader->SetUniform1f("ao", 1.0f);
	renderer->mBackgroundShader->bindShader();
	renderer->mBackgroundShader->SetUniformMat4f("projection", projection);

	renderer->mColorShader->bindShader();
	renderer->mColorShader->SetUniformMat4f("projection", projection);

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	assimpModel = new Model();
	glm::mat4 model = glm::mat4(1.0f);
	renderer->initialize();




#ifdef DEPTH_PASS
#endif // DEPTH_PASS


	// render loop
	// ---------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.getViewMatrix();


		// render scene, supplying the convoluted irradiance map to the final shader.
		// --------------------------------------------------------------------------


		switch (renderer->mRenderMode)
		{

		case RenderContext::RenderMode::WIREFRAME: 
		{
			// Debug WireFrame
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glCullFace(GL_BACK);
			renderer->mColorShader->bindShader();
			renderer->mColorShader->SetUniformMat4f("view", view);
			renderer->mColorShader->SetUniformMat4f("model", model);
			renderer->mColorShader->SetUniform3f("albedo", renderer->mAlbedo);
			assimpModel->Draw();
			break;
				
		};

		case RenderContext::RenderMode::SHADED:
		{
			// Render Shadows
			
			
			// Render PBR
			glViewport(0, 0, scrWidth, scrHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			
			// bind pre-computed IBL data
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->irradianceMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->prefilterMap);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, renderer->brdfLUTTexture);
			
			renderer->mPbrShader->bindShader();
			renderer->mPbrShader->SetUniformMat4f("view", view);
			renderer->mPbrShader->SetUniform3f("camPos", camera.getPosition());
			renderer->mPbrShader->SetUniform3f("albedo", renderer->mAlbedo);
			renderer->mPbrShader->SetUniform1f("roughness", renderer->mRoughness);
			renderer->mPbrShader->SetUniform1f("metallic", renderer->mMetallic);
			renderer->mPbrShader->SetUniformMat4f("model", model);
			
			// Light 0
			if (renderer->mEnableLight0)
			{
				renderer->mPbrShader->SetUniform3f("lightPositions[0]", renderer->mLightPos0);
				renderer->mPbrShader->SetUniform3f("lightColors[0]", renderer->mLightColor0);
			}
			else
			{
				renderer->mPbrShader->SetUniform3f("lightColors[0]", glm::vec3(0.0));
			}

			// Light 1
			if (renderer->mEnableLight1)
			{
				renderer->mPbrShader->SetUniform3f("lightPositions[1]", renderer->mLightPos1);
				renderer->mPbrShader->SetUniform3f("lightColors[1]", renderer->mLightColor1);
			}
			else
			{
				renderer->mPbrShader->SetUniform3f("lightColors[1]", glm::vec3(0.0));
			}
			// Light 2
			if (renderer->mEnableLight2)
			{
				renderer->mPbrShader->SetUniform3f("lightPositions[2]", renderer->mLightPos2);
				renderer->mPbrShader->SetUniform3f("lightColors[2]", renderer->mLightColor2);
			}
			else
			{
				renderer->mPbrShader->SetUniform3f("lightColors[2]", glm::vec3(0.0));
			}
			assimpModel->Draw();
			break;
		};

		case RenderContext::RenderMode::DEPTH:
		{
			// Render Shadows
			glViewport(0, 0, renderer->SHADOW_WIDTH, renderer->SHADOW_HEIGHT);
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, renderer->depthMapFBO));
			glClear(GL_DEPTH_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, renderer->depthMap);

			float near_plane = 1.0f, far_plane = 7.5f;
			glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			glm::mat4 lightView = glm::lookAt(renderer->mLightPos0, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;

			renderer->mShadowDepth->bindShader();
			renderer->mShadowDepth->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
			renderer->mShadowDepth->SetUniformMat4f("model", model);
			assimpModel->Draw();

			// Draw Debug Texture.
			glViewport(0, 0, scrWidth, scrHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderer->mDebugDepthShader->bindShader();
			renderer->mDebugDepthShader->SetUniform1i("depthMap", 0);
			renderer->renderQuad();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			break;
		}

		}

		// Render Background
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		renderer->mBackgroundShader->bindShader();
		renderer->mBackgroundShader->SetUniformMat4f("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->envCubemap);
		renderer->renderCube();


		ProcessUI();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	delete(renderer);
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	//if (!processMouse)
	//{
	//	return;
	//}

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (state == GLFW_PRESS)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------

std::string openfilename(const char* filter, const char* initialDir="") 
{
	// Open File Dialog.
	IFileOpenDialog* pFileOpen;
	IShellItem* pStartDir = nullptr;

	// Setup the initial Directory to start in.
	char absolutePath[MAX_PATH];
	char* fileExt;
	GetFullPathName(TEXT(initialDir), MAX_PATH, absolutePath, &fileExt);
	LOG_INFO("Opening Initial Directory {}", absolutePath);
	
	
	// Need to convert char to wide char for windows.
	const size_t cSize = strlen(absolutePath) + 1;
	std::wstring wc(cSize, L'#');
	mbstowcs(&wc[0], absolutePath, cSize);
	
	// Create the FileOpenDialog object.
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	hr = SHCreateItemFromParsingName(wc.c_str(), NULL, IID_IShellItem, (void**) &pStartDir);
	if (!SUCCEEDED(hr))
	{
		LOG_WARNING("Failed to create start dir shell item. Directory might not exist? {}", absolutePath);
	}

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
	if (!SUCCEEDED(hr))
	{
		LOG_ERROR("Failed to create CoInitializeEx.");
		return "";
	}

	hr = pFileOpen->SetDefaultFolder(pStartDir);
	if (!SUCCEEDED(hr))
	{
		LOG_ERROR("Failed to set Default Folder!");
		return "";
	}
	hr = pFileOpen->SetFolder(pStartDir);

	hr = pFileOpen->Show(NULL);
	if (!SUCCEEDED(hr))
	{
		LOG_WARNING("Failed to get file. User likely closed the dialog.");
		return "";
	}
	IShellItem* pitem;
	hr = pFileOpen->GetResult(&pitem);
	if (!SUCCEEDED(hr))
	{
		LOG_ERROR("Failed to Show Dialog");
		return "";
	}

	PWSTR pszFilePath;
	hr = pitem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (!SUCCEEDED(hr))
	{
		LOG_ERROR("Failed to get File Path");
		pitem->Release();
		CoUninitialize();
		return "";
	}

	char output[MAX_PATH];
	wcstombs(output, pszFilePath, MAX_PATH);
	CoTaskMemFree(pszFilePath);
	pitem->Release();
	CoUninitialize();

	LOG_INFO("Opening File {}", output);
	return output;

}

const char* pfilter = "All Files (*.*)\0*.*\0";

void loadIBL(std::string fileNameStr = "")
{
	const char* initialDir = "..\\..\\extern\\resources\\ibl";

	if (!fileNameStr.size())  // Open File dialog if no filename is provided.
	{
		fileNameStr = openfilename(pfilter, initialDir).c_str();
	}
	if (fileNameStr.size())
	{
		renderer->loadIBL(fileNameStr.c_str());
	}
	else
	{
		renderer->clearIBL();
	}

}

void loadFBX(std::string fileNameStr)
{
	const char* fbxDir = "..\\..\\resources\\fbx";

	if (!fileNameStr.size())  // Open File dialog if no filename is provided.
	{
		fileNameStr = openfilename(pfilter, fbxDir).c_str();
	}
	assimpModel->LoadModel(fileNameStr.c_str());
}

int drawMode = 0;

void ProcessUI()
{

	/////////////////  IMGUI  /////////////////

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{

		ImGui::Begin(" ");                          // Create a window called "Hello, world!" and append into it.
		//ImGui::SetWindowPos(ImVec2(0.9f, 1.0f), ImGuiCond_FirstUseEver);

		// Load IBL
		if (ImGui::Button("Load IBL"))
		{
			loadIBL();
		}

		if (ImGui::Button("Load FBX"))
		{
			loadFBX();
		}

		// TODO: Swap out Shading with renderer.
		bool modeChanged = ImGui::Combo("Draw Mode", &drawMode, renderer->mRenderModeNames, IM_ARRAYSIZE(renderer->mRenderModeNames));
		if (modeChanged) { renderer->setRenderMode(static_cast<RenderContext::RenderMode>(drawMode)); }

		ImGui::ColorEdit3("Albedo", &renderer->mAlbedo.x);
		ImGui::SliderFloat("Roughness", &renderer->mRoughness, 0.0f, 1.0f);
		ImGui::SliderFloat("Metallic", &renderer->mMetallic, 0.0f, 1.0f);


		// Light 0
		ImGui::Checkbox("Enable Light 0", &renderer->mEnableLight0);
		if (renderer->mEnableLight0)
		{
			ImGui::ColorEdit3("Light 0 Color", &renderer->mLightColor0.x);
			ImGui::SliderFloat3("Light 0 Position", &renderer->mLightPos0[0], -10.0, 10.0);
		}
		// Light 1
		ImGui::Checkbox("Enable Light 1", &renderer->mEnableLight1);
		if (renderer->mEnableLight1)
		{
			ImGui::ColorEdit3("Light 1 Color", &renderer->mLightColor1.x);
			ImGui::SliderFloat3("Light 1 Position", &renderer->mLightPos1[1], -10.0, 10.0);
		}
		// Light 2
		ImGui::Checkbox("Enable Light 2", &renderer->mEnableLight2);
		if (renderer->mEnableLight2)
		{
			ImGui::ColorEdit3("Light 2 Color", &renderer->mLightColor2.x);
			ImGui::SliderFloat3("Light 2 Position", &renderer->mLightPos2[2], -10.0, 10.0);
		}




		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}





