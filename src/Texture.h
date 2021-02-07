#ifndef _TEXTURE_H
#define _TEXTURE_H


#include "Debugging.h"
#include <string>
#include <vector>
#include "Log.h"
#include "stb/stb_image.h"


class BaseTexture
{
protected:
	unsigned int mTextureID;
	unsigned int mSlot;
public:
	virtual unsigned int GetSlot();
	virtual void BindTexture(unsigned int pSlot = 0);
	virtual void Unbind();
};


class Skybox : public BaseTexture
{
public:
	Skybox();
	Skybox(std::vector<std::string> faces);
	~Skybox();

	protected:
	std::vector<std::string> m_Faces;
	void LoadCubemap();

};


class Texture : public BaseTexture
{
private:
	std::string mFilePath;
	unsigned char* mLocalBuffer;
	int m_Width, m_Height, m_BPP;

public:
	Texture(const std::string& path);
	~Texture();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

};







#endif
