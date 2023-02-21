#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"



void createDepthFBO(unsigned int pWidth, unsigned int pHeight, unsigned int& fbo, unsigned int& depthMap)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Attach Texture

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, pWidth, pHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



unsigned int BaseTexture::getSlot()
{
	if (!mInitialized)
	{
		LOG_ERROR("Texture is not initialized!");
		return 0;
	}
	else
	{
		return mSlot;
	}

}

void BaseTexture::bindTexture(unsigned int pSlot /*= 0*/)
{
	mSlot = pSlot;
	glActiveTexture(GL_TEXTURE0 + mSlot);
	GLCall(glBindTexture(GL_TEXTURE_2D, mTextureID));
}

void BaseTexture::unbindTexture()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}



Texture::Texture(const char* path)
	: mFilePath(path), mLocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	std::filesystem::path _path = std::filesystem::absolute(path);
	LOG_INFO("Loading Texture {}", _path.string());

	bool _isHDR = (_path.extension() == ".hdr");
	if (_isHDR)
	{
		LOG_INFO("Texture is hdr!");
	}
	
	if (_isHDR)
	{
		stbi_set_flip_vertically_on_load(true);

		float* data = stbi_loadf(_path.string().c_str(), &m_Width, &m_Height, &m_BPP, 0);


		if (!data)
		{
			LOG_ERROR("Could not find texture! {}", _path.string());
		}
		//stbi_set_flip_vertically_on_load(true);
		GLCall(glGenTextures(1, &mTextureID));
		GLCall(glBindTexture(GL_TEXTURE_2D, mTextureID));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, data));
	
		if (data)
		{
			stbi_image_free(data);
		}
	}
	else
	{
		stbi_set_flip_vertically_on_load(true);
		mLocalBuffer = stbi_load(_path.string().c_str(), &m_Width, &m_Height, &m_BPP, 3);


		if (!mLocalBuffer)
		{
			LOG_ERROR("Could not find texture! {}", _path.string());
		}

		GLCall(glGenTextures(1, &mTextureID));
		GLCall(glBindTexture(GL_TEXTURE_2D, mTextureID));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, mLocalBuffer));
		if (mLocalBuffer)
		{
			stbi_image_free(mLocalBuffer);
		}

	}

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	mInitialized = true;

}

Texture::Texture(unsigned int size)
{
	GLCall(glGenTextures(1, &mTextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, mTextureID));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, size, size, 0, GL_RG, GL_FLOAT, nullptr);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	mInitialized = true;

}

Texture::~Texture()
{
	if (glIsTexture(mTextureID))
	{
		GLCall(glDeleteTextures(1, &mTextureID));
	}
}



Cubemap::Cubemap(std::vector<std::string> faces)
	: m_Faces(faces)
{
	LoadCubemap();
}

Cubemap::Cubemap(unsigned int size, bool filterMipmap)
{
	LOG_INFO("Generating Cubemap {} {}", size, filterMipmap);
	// TODO Add construction parameters.
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);
	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (filterMipmap)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combating visible dots artifact)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	mInitialized = true;
	
}

Cubemap::~Cubemap()
{
	GLCall(glDeleteTextures(1, &mTextureID));
}


void Cubemap::bindTexture(unsigned int pSlot /*= 0*/)
{
	mSlot = pSlot;
	glActiveTexture(GL_TEXTURE0 + mSlot);
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID));

}

void Cubemap::LoadCubemap()
{
	if (m_Faces.empty())
	{
		LOG_ERROR("No texture paths assigned to SkyBox!");
		return;
	}
	//unsigned int testID;
	glGenTextures(1, &mTextureID);
	bindTexture(mTextureID);

	int width, height, nrchannels;

	for (unsigned int i = 0; i < m_Faces.size(); i++)
	{
		stbi_set_flip_vertically_on_load(0);
		unsigned char *data = stbi_load(m_Faces[i].c_str(), &width, &height, &nrchannels, 3);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			
		}
		else
		{
			LOG_ERROR("Could not load image!! {}", m_Faces[i].c_str());
			return;

		}

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	mInitialized = true;
	//GLCall(glGenTextures(1, &m_TextureID));
}


DepthFBO::~DepthFBO()
{
	glDeleteTextures(1, &mTexture);
	glDeleteFramebuffers(1, &mFBO);
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
	unbindFBO();
	unbindTexture();
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

	unbindFBO();
	unbindRBO();
}


void CaptureFBO::initialize(int pWidth, int pHeight)
{
	glGenFramebuffers(1, &mFBO);
	glGenRenderbuffers(1, &mRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

}
