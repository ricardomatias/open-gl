#pragma once
#define DEBUG

#include <GL/glew.h>

#define ASSERT(x) \
    if (!(x))     \
    __builtin_debugtrap()

#ifdef DEBUG
#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x;
#endif

#define LOG(x) std::cout << (x) << std::endl

void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);
