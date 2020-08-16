#ifndef _TEXTURE_H
#define _TEXTURE_H


#include "Debugging.h"
#include <string>
#include <vector>
#include "Log.h"
#include "stb/stb_image.h"


class Skybox
{
public:
	Skybox(std::vector<std::string> faces);
	~Skybox();

	void BindTexture(unsigned int slot = 0) const;
	void Unbind() const;


private:

	std::vector<std::string> m_Faces;
	unsigned int m_TextureID = 0;
	void LoadCubemap();

};


class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;

public:
	Texture(const std::string& path);
	~Texture();

	void BindTexture(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

};







#endif
