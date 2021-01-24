#pragma once 
#include "GL/glew.h"
#include "Log.h"
#include "Scene.h"




class RenderContext
{
public:

	RenderContext(const SceneContext& pScene);
	~RenderContext();


	void init(GLuint pScreenWidth, GLuint pScreenHeight);
	void onDisplay();

	void setWindowSize(GLuint pScreenWidth, GLuint pScreenHeight);


	GLFWwindow* window();


	// Render Options
	bool wireFrameOnShaded = false;
	bool drawReflections = true;
	bool drawShadows = true;
	bool drawTextures = false;
	bool drawNormals = true;
	bool drawSky = true;
	bool drawDebug = false;

	// UI Options
	bool processMouse = true;

private:
	const SceneContext& mScene;
	bool mInitialized = false;

	// Window
	GLFWwindow *mWindow;
	GLuint mScreenWidth;
	GLuint mScreenHeight;

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