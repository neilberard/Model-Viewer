#pragma once

#include "Debugging.h"
#include <string>
#include <vector>
#include "Log.h"
#include "stb/stb_image.h"
#include <filesystem>
#include <iostream>


struct TextureID {
	unsigned int id;
	std::string type;
};

class BaseTexture
{
public:
	~BaseTexture() { glDeleteTextures(1, &mTextureID); }

	unsigned int getSlot();
	virtual unsigned int getID() { return mTextureID; }
	virtual void bindTexture(unsigned int pSlot = 0);
	virtual void unbindTexture();

protected:
	bool mInitialized = false;
	unsigned int mTextureID = 0;
	unsigned int mSlot = 0;
};


class Cubemap : public BaseTexture
{
public:
	Cubemap() {};
	Cubemap(unsigned int size, bool filterMipmap=false);
	Cubemap(std::vector<std::string> faces);
	~Cubemap();

	virtual void bindTexture(unsigned int pSlot = 0);

	protected:
	std::vector<std::string> m_Faces;
	void LoadCubemap();

};


class Texture : public BaseTexture
{
private:
	std::filesystem::path mFilePath;
	unsigned char* mLocalBuffer;
	int m_Width, m_Height, m_BPP;

public:
	Texture(unsigned int size);
	Texture(const char* path);
	~Texture();

	inline int getWidth() const { return m_Width; }
	inline int getHeight() const { return m_Height; }
};


class DepthFBO
{
public:
	
	
	DepthFBO(int pWidth, int pHeight) { initialize(pWidth, pHeight); }
	~DepthFBO();

	void initialize(int pWidth, int pHeight);

	void bindTexture()		{ GLCall(glBindTexture(GL_TEXTURE_2D, mTexture)); }
	void unbindTexture()	{ GLCall(glBindTexture(GL_TEXTURE_2D, 0)); };

	void bindFBO()			{ GLCall(glBindFramebuffer(GL_FRAMEBUFFER, mFBO)); };
	void unbindFBO()		{ GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0)); };

	GLuint mTexture = 0;

private:
	
	GLuint mFBO;
	GLuint mRBO;
	GLuint mRBODepth;

	int mWidth;
	int mHeight;
};

// Color id
class ColorFBO
{
public:
	ColorFBO(int pWidth, int pHeight) { initialize(pWidth, pHeight); }

	void initialize(int pWidth, int pHeight);
	void bindFBO() { GLCall(glBindFramebuffer(GL_FRAMEBUFFER, mFBO)); }
	void bindRBO() { GLCall(glBindRenderbuffer(GL_RENDERBUFFER, mRBO)); }
	void bindRBODepth() { GLCall(glBindFramebuffer(GL_RENDERBUFFER, mRBODepth)); }

	void unbindFBO() { GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0)); }
	void unbindRBO() { GLCall(glBindRenderbuffer(GL_RENDERBUFFER, mRBO)); }


private:
	GLuint mFBO = 0;
	GLuint mRBO = 0;
	GLuint mRBODepth = 0;

	int mWidth =  0;
	int mHeight = 0;

};


class CaptureFBO
{
public:
	CaptureFBO(int pWidth, int pHeight) { initialize(pWidth, pHeight); }
	
	void initialize(int pWidth, int pHeight);
	
	void bindFBO()		{ GLCall(glBindFramebuffer(GL_FRAMEBUFFER, mFBO)); }
	void bindRBO()		{ GLCall(glBindRenderbuffer(GL_RENDERBUFFER, mRBO)); }

	void unbindFBO()	{ GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0)); }
	void unbindRBO()	{ GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0)); }

private:
	GLuint mFBO = 0;
	GLuint mRBO = 0;

};