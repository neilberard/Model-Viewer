#pragma once


class IndexBuffer
{

private:
	unsigned int m_RendererID;
	unsigned int m__Count;
public:

	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void UnBind() const;

	inline unsigned int GetCount() const { return m__Count; }

};