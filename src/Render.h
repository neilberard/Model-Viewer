#pragma once 
#include "GL/glew.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include <map>
#include <memory>


namespace Render
{
	void _renderCube();
	void _renderQuad();
}


class RenderContext
{
public:
	// GLFW must be initialized before instantiating this class.
	RenderContext() {};
	~RenderContext() { LOG_INFO("Destroying Render Context!"); }
	RenderContext(GLFWwindow* pWindow);

	void initialize();



	bool mBuffersInitialized = false;

	// For UI selection. These Names should match the RenderMode Enum names.
	const char* mRenderModeNames[3]{ "Wireframe", "Shaded", "Depth"};

	enum RenderMode
	{
		WIREFRAME = 0,
		SHADED    = 1,
		DEPTH     = 2,
	};

	RenderMode mRenderMode = WIREFRAME;

	float mShadowBias = 0.001;
	float mEnvironmentLightIntensity = 1.0;
	
	// Shader Parameter Values.
	glm::vec3 mAlbedo = glm::vec3(0.5);
	float mRoughness = 0.5;
	float mMetallic = 0.5;

	void setRenderMode(int pMode) { mRenderMode = static_cast<RenderMode>(pMode);}

	void resize();
	void reloadShaders();

	// Render Cube
	static unsigned int cubeVAO;
	static unsigned int cubeVBO;

	static void renderCube();
	static void renderQuad() { Render::_renderQuad(); }

	// Render Sphere
	static unsigned int sphereVAO;
	static unsigned int sphereVbo;
	static unsigned int sphereEbo;
	static unsigned int sphereIndexCount;
	static void renderSphere();

	void loadIBL(const char* filePath);
	void clearIBL();

	bool isInitialized() { return mInitialized; }

	int mSelected = 0;

	unsigned int mShadowResolution = 1024;

	glm::mat4 mLightSpace = glm::mat4();
	glm::mat4 mModelSpace = glm::mat4();

	// Render Options
	bool mEnableLight0 = false;
	glm::vec3 mLightColor0 = glm::vec3(1.0);
	glm::vec3 mLightPos0 = glm::vec3();

	bool mEnableLight1 = false;
	glm::vec3 mLightColor1 = glm::vec3(1.0);
	glm::vec3 mLightPos1 = glm::vec3();

	bool mEnableLight2 = false;
	glm::vec3 mLightColor2 = glm::vec3(1.0);
	glm::vec3 mLightPos2 = glm::vec3();


	// Shaders
	std::unique_ptr<Shader> mPbrShader = nullptr;
	std::unique_ptr<Shader> mBackgroundShader = nullptr;
	std::unique_ptr<Shader> mDepthShader = nullptr;
	std::unique_ptr<Shader> mPostShader = nullptr;
	std::unique_ptr<Shader> mSkyShader = nullptr;
	std::unique_ptr<Shader> mColorShader = nullptr;
	std::unique_ptr<Shader> brdfShader = nullptr;
	// Debug Shaders
	std::unique_ptr<Shader> mDebugDepthShader = nullptr;
	
	// FRAME BUFFERS
	DepthFBO* mDepthFBO = nullptr;
	ColorFBO* mColorFBO = nullptr;

	std::unique_ptr<Cubemap> mEnvCubeMap = nullptr;
	std::unique_ptr<Cubemap> mIrradianceCubeMap = nullptr;
	std::unique_ptr<Cubemap> mPrefilterCubeMap = nullptr;


	unsigned int captureFBO, captureRBO;
	unsigned int irradianceMap;
	unsigned int envCubemap;
	unsigned int brdfLUTTexture;
	unsigned int hdrTexture;
	unsigned int prefilterMap;

	// Shadows
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int depthMapFBO;
	unsigned int depthMap;
	std::unique_ptr<Shader> mShadowDepth = nullptr;

	int mWindowWidth = 0;
	int mWindowHeight = 0;


private:

	std::map<const char*, Shader*> mShaderMap;

	bool mInitialized = false;
	GLFWwindow* mWindow;
	const Model* mModel = nullptr;

	std::unique_ptr<Shader> _prefilterShader = nullptr;
	std::unique_ptr<Shader> _equirectangularToCubemapShader = nullptr;
	std::unique_ptr<Shader> _irradianceShader = nullptr;


	// Render Process shaders 
	//--------------------------------------//

	Cubemap mSkyBox;
	//Shader mShadowShader;
};

