#ifndef _DEBUGGING_H
#define _DEBUGGING_H

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string>



#define LOGGER Logger::GetLogger()


/*
GLCALL is a simple wrapper that will break during a debug session when passed opengl function returns an error.

EXAMPLE USAGE: 	GLCall(glGenBuffers(1, &VBO));
*/

#ifdef _DEBUG


#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define ASSERT(x);
#define GLCall(x) x;
#endif

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);




#endif