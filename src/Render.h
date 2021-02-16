#pragma once 
#include "GL/glew.h"
#include "Log.h"
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Debugging.h"
#include <memory>




class DepthFBO
{
public:
	DepthFBO() {};
	DepthFBO(int pWidth, int pHeight);
	void initialize(int pWidth, int pHeight);
	void bind();
	void bindFBO() { glBindFramebuffer(GL_FRAMEBUFFER, mFBO); };
	void bindTexture(int activeLevel = 0);

	void unbind();
	void unbindFBO() { glBindFramebuffer(GL_FRAMEBUFFER, 0); };
	void UnbindTexture() { glBindTexture(GL_TEXTURE_2D, 0); };


private:
	unsigned int mFBO;
	unsigned int mTexture;

	int mWidth;
	int mHeight;

};

// Color id
class ColorFBO
{
public:
	ColorFBO() {};
	ColorFBO(int pWidth, int pHeight);
	void initialize(int pWidth, int pHeight);
	void bind();
	void bindFBO() { glBindFramebuffer(GL_FRAMEBUFFER, mFBO); };
	void bindTexture(int activeLevel = 0);

	void unbind();
	void unbindFBO() { glBindFramebuffer(GL_FRAMEBUFFER, 0); };
	void unbindTexture() { glBindTexture(GL_TEXTURE_2D, 0); };


private:
	unsigned int mFBO;
	unsigned int mRBO;
	unsigned int mRBODepth;
	unsigned int mTexture;

	int mWidth;
	int mHeight;

};


class RenderContext
{
public:
	// GLFW must be initialized before instantiating this class.
	RenderContext(const SceneContext* pScene, GLFWwindow* pWindow, const Model* pModel, const unsigned int* pBlock);
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
	void renderColorIds();

	void selectObject(double xpos, double ypos);
	int mSelected = -1;

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
	Shader mDepthShader = Shader("../../resources/shaders/simpleDepthShader.glsl");
	Shader mPostShader = Shader("../../resources/shaders/post.glsl");
	Shader mSkyShader = Shader("../../resources/shaders/cubemap.glsl");
	Shader mDiffuseShader = Shader("../../resources/shaders/shadows.glsl");
	Shader mColorShader = Shader("../../resources/shaders/color.glsl");

	DepthFBO mDepthFBO = DepthFBO(mShadowResolution, mShadowResolution);
	ColorFBO mColorFBO;
	SimpleCube mSkyCube = SimpleCube(1.0, false);
	const unsigned int* mUBO = 0; //Uniform Buffer Object


	// UI Options
	bool mProcessMouse = true;

private:
	int mWidth;
	int mHeight;

	bool mInitialized = false;
	const SceneContext* mScene;
	GLFWwindow* mWindow;
	const Model* mModel = nullptr;


	std::vector<std::string>mCubemap;
	std::string mCubeMapFormat = std::string("jpg");
	std::string mCubeMapDir = std::string("../../extern/resources/textures/skyA/");
	// Render Process shaders 
	//--------------------------------------//

	Skybox mSkyBox;
	//Shader mShadowShader;


	Texture mDiffuseMap = Texture("../../extern/resources/textures/Medieval_1K_diffuse.png");
	Texture mNormalMap = Texture("../../extern/resources/textures/brickwall_normal.jpg");



	// TEXTURE SLOTS
	int mSkyTextureSlot = 0;
	int mShadowTextureSlot = 1;
	int mDepthTextureSlot = 2;
	int mNormalTextureSlot = 3;
	int mDiffuseTextureSlot = 4;
};