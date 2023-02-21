#pragma once 
#include "GL/glew.h"
#include "Log.h"
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Debugging.h"
#include <memory>


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

	enum RenderDebug
	{
		DEBUG_OFF = 0,
		NORMAL = 1,
		WIREFRAME = 2,
	};

	RenderDebug mRenderDebug = DEBUG_OFF;


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
	void loadIBL(const char* filePath);

	bool isInitialized() { return mInitialized; }

	int mSelected = 0;

	unsigned int mShadowResolution = 1024;

	glm::mat4 mLightSpace = glm::mat4();
	glm::mat4 mModelSpace = glm::mat4();

	// Render Options
	bool mWireFrameOnShaded = false;
	bool mDrawReflections = true;
	bool mDrawShadows = true;
	bool mDrawTextures = false;
	bool mDrawNormals = true;
	bool mDrawSky = true;
	bool mDrawDebug = false;


	// Leaving these public until I can decouple set uniforms
	std::vector<Shader*> mShaders;

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
	CaptureFBO* mCaptureFBO = 0;

	DepthFBO* mDepthFBO = nullptr;
	ColorFBO* mColorFBO = nullptr;

	// PBR IBL ----------------------------------------------------
	Texture* mHdrMap = nullptr;
	Texture* mBrdfLUTTexture = nullptr;
	
	Cubemap* mEnvCubeMap = nullptr;
	Cubemap* mIrradianceCubeMap = nullptr;
	Cubemap* mPrefilterCubeMap = nullptr;
	unsigned int brdfLUTTexture;

	//Shader* mEquirectangularToCubemapShader = nullptr;
	//Shader* mIrradianceShader = nullptr;
	//Shader* mPrefilterShader = nullptr;
	//Shader* mBrdfShader = nullptr;
	Shader* _prefilterShader = nullptr;


	// UI Options
	bool mProcessMouse = true;


	unsigned int captureFBO, captureRBO;
	unsigned int irradianceMap;
	unsigned int envCubemap;
	unsigned int hdrTexture;
	unsigned int prefilterMap;



private:
	int mWidth = 0;
	int mHeight = 0;

	bool mInitialized = false;
	SceneContext* mScene;
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

