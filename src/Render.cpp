#include "Render.h"


RenderContext::RenderContext(const SceneContext* pScene, GLFWwindow* pWindow, const Model* pModel, const unsigned int* pBlock) : 
	mScene(pScene), mWindow(pWindow), mModel(pModel), mUBO(pBlock)
{
	// Setup Shaders

	mCubemap = std::vector<std::string>{
		    mCubeMapDir + "right" + "." + mCubeMapFormat,
			mCubeMapDir + "left" + "." + mCubeMapFormat,
			mCubeMapDir + "top" + "." + mCubeMapFormat,
			mCubeMapDir + "bottom" + "." + mCubeMapFormat,
			mCubeMapDir + "front" + "." + mCubeMapFormat,
			mCubeMapDir + "back" + "." + mCubeMapFormat
	};

	mSkyBox = Skybox(mCubemap);
	mSkyBox.BindTexture(mSkyTextureSlot);

	// Set UBO
	mSkyShader.Bind();
	mSkyShader.SetUniform1i("uBlock", *mUBO);
	mSkyShader.UnBind();

	mDiffuseShader.Bind();
	mDiffuseShader.SetUniform1i("uBlock", *mUBO);
	mDiffuseShader.UnBind();

	mColorShader.Bind();
	mColorShader.SetUniform1i("uBlock", *mUBO);
	mColorShader.UnBind();


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

}

void RenderContext::renderShadows()
{
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glViewport(0, 0, mShadowResolution, mShadowResolution);

	mDepthFBO.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	mDepthShader.Bind();
	mDepthShader.SetUniformMat4f("u_LightSpaceMatrix", mLightSpace);
	mDepthShader.SetUniformMat4f("u_Model", mModelSpace);

	for (unsigned int i = 0; i < mModel->mMeshes.size(); i++)
	{
		mModel->mMeshes[i]->draw();
	}

	mDepthShader.UnBind();
	mDepthFBO.unbind();
}

void RenderContext::renderDiffuse()
{
	mDiffuseShader.Bind();
	mDepthFBO.BindTexture(mDepthTextureSlot);
	mDiffuseMap.BindTexture(mDiffuseTextureSlot);
	mNormalMap.BindTexture(mNormalTextureSlot);

	mDiffuseShader.SetUniform1i("uNormalMap", mNormalTextureSlot);
	mDiffuseShader.SetUniform1i("uDiffuseMap", mDiffuseTextureSlot);
	mDiffuseShader.SetUniform1i("uShadowMap", mDepthTextureSlot);
	mDiffuseShader.SetUniform1i("uSky", mSkyTextureSlot);
	int width, height;
	glfwGetWindowSize(mWindow, &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	onDisplay();

}

void RenderContext::renderWireframe()
{
	mColorShader.Bind();
	mColorShader.SetUniform3f("uColor", 1.0f, 0.0f, 0.0f);
	onDisplay();
	mColorShader.UnBind();
}

void RenderContext::renderSky()
{
	glCullFace(GL_FRONT);
	glDepthMask(GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	mSkyBox.BindTexture(mSkyTextureSlot);
	mSkyShader.Bind();
	mSkyShader.SetUniform1i("uSky", mSkyTextureSlot);
	mSkyCube.draw();
	mSkyShader.UnBind();
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
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