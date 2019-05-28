#include "Renderer.h"

#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cerr << "[OpenGL Error]: 0x" << std::hex << error << std::dec << " \n"
                  << function << " " << file << ":" << line << std::endl;
        return false;
    }

    return true;
}
