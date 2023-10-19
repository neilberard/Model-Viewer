#pragma once 
#include "GL/glew.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include <map>


#define _TEST_CODE

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
	RenderContext(GLFWwindow* pWindow);
	~RenderContext();


	// For UI selection. These Names should match the RenderMode Enum names.
	const char* mRenderModeNames[2]{ "Wireframe", "Shaded"};

	enum RenderMode
	{
		WIREFRAME = 0,
		SHADED = 1,
	};

	RenderMode mRenderMode = WIREFRAME;
	
	// Shader Parameter Values.
	glm::vec3 mAlbedo = glm::vec3(0.5);
	float mRoughness = 0.5;
	float mMetallic = 0.5;

	void setRenderMode(int pMode) { mRenderMode = static_cast<RenderMode>(pMode);}
	Shader* addShader(const char* pShader);

	void resize();
	void onDisplay();
	void renderShadows();
	void renderDiffuse();
	void renderWireframe();
	void renderSky();
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



	void renderColorIds();
	void selectObject(double xpos, double ypos);

	int mRenderIBL = 0;
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


	// Leaving these public until I can decouple set uniforms
	Shader* mPbrShader = nullptr;
	Shader* mBackgroundShader = nullptr;
	Shader* mDepthShader = nullptr;
	Shader* mPostShader = nullptr;
	Shader* mSkyShader = nullptr;
	Shader* mColorShader = nullptr;
	Shader* brdfShader = nullptr;
	
	Texture* mDiffuseMap = nullptr;
	Texture* mNormalMap = nullptr;

	SimpleCube* mSkyCube = new SimpleCube(1.0, false);


	// FRAME BUFFERS
	DepthFBO* mDepthFBO = nullptr;
	ColorFBO* mColorFBO = nullptr;

	// PBR IBL ----------------------------------------------------
	Texture* mHdrMap = nullptr;
	Texture* mBrdfLUTTexture = nullptr;
	
	Cubemap* mEnvCubeMap = nullptr;
	Cubemap* mIrradianceCubeMap = nullptr;
	Cubemap* mPrefilterCubeMap = nullptr;
	unsigned int brdfLUTTexture;

	Shader* _prefilterShader = nullptr;


	// UI Options
	unsigned int captureFBO, captureRBO;
	unsigned int irradianceMap;
	unsigned int envCubemap;
	unsigned int hdrTexture;
	unsigned int prefilterMap;


private:

	std::map<const char*, Shader*> mShaderMap;

	int mWidth = 0;
	int mHeight = 0;

	bool mInitialized = false;
	GLFWwindow* mWindow;
	const Model* mModel = nullptr;

	Shader* _equirectangularToCubemapShader;


	// Render Process shaders 
	//--------------------------------------//

	Cubemap mSkyBox;
	//Shader mShadowShader;

	bool mBuffersInitialized = false;

	Shader* _irradianceShader;

};

