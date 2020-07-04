#include "IndexBuffer.h"
#include "Debugging.h"
#include <fstream>

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m__Count(count)
{
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));

}

IndexBuffer::IndexBuffer()
	:m__Count(0)
{

}

IndexBuffer::~IndexBuffer()
{
	printf("DELETING INDEX BUFFER");
	GLCall(glDeleteBuffers(1, &m_RendererID));

}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::UnBind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
