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


	int width, height;
	glfwGetWindowSize(mWindow, &width, &height);
	mColorFBO.initialize(width, height);

}

RenderContext::~RenderContext()
{
	LOG_INFO("Deleting RenderContext. Goodbye!");
}

void RenderContext::resize()
{
	
	glfwGetWindowSize(mWindow, &mWidth, &mHeight);
	mColorFBO.initialize(mWidth, mHeight);


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
	mDepthFBO.bindTexture(mDepthTextureSlot);
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
	glPolygonMode(GL_FRONT, GL_LINE);
	glCullFace(GL_BACK);
	glLineWidth(2.0);
	mColorShader.Bind();
	mColorShader.SetUniform3f("uColor", 1.0f, 0.0f, 0.0f);
	mColorShader.SetUniform3f("uOffset", 0.0f, 0.0f, 0.0005f);

	for (unsigned int i = 0; i < mModel->mMeshes.size(); i++)
	{
		if(mModel->mMeshes[i]->mId == mSelected)
		{
			mModel->mMeshes[i]->draw();	
		}
	}

	glLineWidth(1.0);
	//onDisplay();
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

void RenderContext::renderColorIds()
{


	mColorShader.Bind();
	mColorFBO.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GL_FILL);


	for (unsigned int i = 0; i < mModel->mMeshes.size(); i++)
	{
		float val = 1.0f / 255.0f;
		float color = float(mModel->mMeshes[i]->mId) * val;
		mColorShader.SetUniform3f("uColor", color, color, color);
		mModel->mMeshes[i]->draw();
	}
	mColorFBO.unbind();
	mColorShader.UnBind();
}

void RenderContext::selectObject(double xpos, double ypos)
{
	LOG_INFO("Selecting object {} {}", xpos, ypos);
	mColorFBO.bind();
	
	int width, height;
	glfwGetWindowSize(mWindow, &width, &height);

	unsigned char *image = new unsigned char[(width * height * 3)];
	glReadPixels(GLuint(xpos), GLuint(height - ypos), width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	LOG_INFO("Reading Color Buffer {}", int(image[0]));
	mSelected = int(image[0]);
	delete image;
	mColorFBO.unbind();

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
	bindTexture();
	bindFBO();
}

void DepthFBO::bindTexture(int activeLevel /*= 0*/)
{
	
	glActiveTexture(GL_TEXTURE0 + activeLevel);
	glBindTexture(GL_TEXTURE_2D, mTexture);

}

void DepthFBO::unbind()
{
	unbindFBO();
	UnbindTexture();

}

ColorFBO::ColorFBO(int pWidth, int pHeight)
{
	initialize(pWidth, pHeight);
}

void ColorFBO::initialize(int pWidth, int pHeight)
{

	mWidth = pWidth;
	mHeight = pHeight;

	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	//attach RBO COLOR
	glGenRenderbuffers(1, &mRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRBO);


	glGenRenderbuffers(1, &mRBODepth);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBODepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRBODepth);



	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_DEBUG("Frame buffer Color Fbo complete!");
	}
	else
	{
		LOG_ERROR("No Color Frame buffer!");
	}
	unbind();

}

void ColorFBO::bind()
{
	bindFBO();
}

void ColorFBO::bindTexture(int activeLevel /*= 0*/)
{
	glActiveTexture(GL_TEXTURE0 + activeLevel);
	glBindTexture(GL_TEXTURE_2D, mTexture);

}

void ColorFBO::unbind()
{
	unbindFBO();


}