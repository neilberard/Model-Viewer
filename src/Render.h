#pragma once 
#include "GL/glew.h"
#include "Log.h"



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