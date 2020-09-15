#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"


float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};



unsigned int BaseTexture::GetSlot()
{
	return mSlot;
}

void BaseTexture::BindTexture(unsigned int pSlot /*= 0*/)
{
	mSlot = pSlot;
	glActiveTexture(GL_TEXTURE0 + mSlot);
	GLCall(glBindTexture(GL_TEXTURE_2D, mTextureID));
}

void BaseTexture::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}



Texture::Texture(const std::string& path)
	: mFilePath(path), mLocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{

	stbi_set_flip_vertically_on_load(1);
	mLocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 3);

	if (!mLocalBuffer)
	{
		LOG_ERROR("Could not find texture! {}", path);
	}

	GLCall(glGenTextures(1, &mTextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, mTextureID));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, mLocalBuffer));
	if (mLocalBuffer)
	{
		stbi_image_free(mLocalBuffer);
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &mTextureID));
}



Skybox::Skybox(std::vector<std::string> faces)
	: m_Faces(faces)
{
	LoadCubemap();
}

Skybox::~Skybox()
{
	GLCall(glDeleteTextures(1, &mTextureID));
}


void Skybox::LoadCubemap()
{
	if (m_Faces.empty())
	{
		LOG_ERROR("No texture paths assigned to SkyBox!");
		return;
	}
	//unsigned int testID;
	glGenTextures(1, &mTextureID);
	BindTexture(mTextureID);

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

	
	//GLCall(glGenTextures(1, &m_TextureID));
}

