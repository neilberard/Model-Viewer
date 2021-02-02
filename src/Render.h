#pragma once 
#include "GL/glew.h"
#include "Log.h"
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include <memory>


class RenderContext
{
public:
	// GLFW must be initialized before instantiating this class.
	RenderContext(const SceneContext* pScene, const GLFWwindow* pWindow, const Model* pModel);
	~RenderContext();

	void onDisplay();


	// Render Options
	bool mWireFrameOnShaded = false;
	bool drawReflections = true;
	bool mDrawShadows = true;
	bool drawTextures = false;
	bool drawNormals = true;
	bool drawSky = true;
	bool drawDebug = false;

	// UI Options
	bool processMouse = true;

private:
	const SceneContext* mScene;
	const GLFWwindow* mWindow;
	const Model* mModel = nullptr;
	bool mInitialized = false;

	// Stores all meshes


	// Window
	std::unique_ptr<Shader> mDepthShader;

};




class DepthFBO
{
public:
	DepthFBO(int pWidth, int pHeight);
	void initialize(int pWidth, int pHeight);
	void bind();
	void bindFBO() { glBindFramebuffer(GL_FRAMEBUFFER, mFBO); };
	void BindTexture(int activeLevel = 0);

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
	unsigned int mTexture;

	int mWidth;
	int mHeight;

};