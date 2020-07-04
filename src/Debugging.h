
/* Logger is Based on code from https://cppcodetips.wordpress.com/2014/01/02/a-simple-logger-class-in-c/ by Sadique Ali E
*/


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



/*
SIMPLE LOGGER
*/

enum LogLeval
{
	ERROR, WARNING, INFO, DEBUG
};

class Logger
{

private:
	static Logger* m_pThis;


public:

	Logger();
	virtual ~Logger();
	
	void log(const std::string& message);
	void log(const char* format, ...);


	static Logger* GetLogger();


};





#endif