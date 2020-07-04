#include "Debugging.h"
Logger* Logger::m_pThis = NULL;

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{

	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ")" << function << file << ":" << line << std::endl;
		return false;
	}
	return true;

}

Logger::Logger()
{

}

Logger::~Logger()
{

}

void Logger::log(const std::string& message)
{
	printf("%s \n", message.c_str());
}

void Logger::log(const char* format, ...)
{
	
	char* sMessage = NULL;
	int nLength = 0;
	va_list args;
	va_start(args, format);


	/* Return the number of characters in a string referenced the list of arguments.
	_vsprintf doesn't count terminating character, add +1 to count for that
	*/	

	nLength = _vscprintf(format, args) + 1;
	sMessage = new char[nLength];
	vsprintf_s(sMessage, nLength, format, args);
	std::cout << sMessage << std::endl;
	va_end(args);


}

Logger* Logger::GetLogger()
{
	if (m_pThis == NULL)
	{
		m_pThis = new Logger();
	}
	return m_pThis;

}
