#include "Render.h"


RenderContext::RenderContext(const SceneContext* pScene, const GLFWwindow* pWindow, const Model* pModel) : 
	mScene(pScene), mWindow(pWindow), mModel(pModel)
{

	// Setup Shaders
	mDepthShader = std::make_unique<Shader>("../../resources/shaders/simpleDepthShader.glsl");
}

RenderContext::~RenderContext()
{
	LOG_INFO("Deleting RenderContext. Goodbye!");

}

void RenderContext::onDisplay()
{

	if (mModel == nullptr || !mModel->mLoaded)
	{
		return;
	}


	for (unsigned int i = 0; i < mModel->mMeshes.size(); i++)
	{
		mModel->mMeshes[i]->draw();
	}


	if (mWireFrameOnShaded)
	{
		LOG_INFO("Wireframe ", mWireFrameOnShaded);
	}


	// Depth FBO








}


DepthFBO::DepthFBO(int pWidth, int pHeight)
{
	initialize(pWidth, pHeight);
}

void DepthFBO::initialize(int pWidth, int pHeight) 
{
	mWidth = pWidth;
	mHeight = pHeight;

	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);	
	
	// Attach Texture

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_DEBUG("Frame buffer Fbo complete!");
	}
	else
	{
		LOG_ERROR("No Frame buffer!");
	}
	unbind();

}

void DepthFBO::bind()
{
	BindTexture();
	bindFBO();
}

void DepthFBO::BindTexture(int activeLevel /*= 0*/)
{
	glActiveTexture(GL_TEXTURE0 + activeLevel);
	glBindTexture(GL_TEXTURE_2D, mTexture);

}

void DepthFBO::unbind()
{
	unbindFBO();
	UnbindTexture();

}

void ColorFBO::initialize(int pWidth, int pHeight)
{

	mWidth = pWidth;
	mHeight = pHeight;

	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// Attach Texture

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_DEBUG("Frame buffer Fbo complete!");
	}
	else
	{
		LOG_ERROR("No Frame buffer!");
	}
	unbind();

}

void ColorFBO::unbind()
{

}