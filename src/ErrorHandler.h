#pragma once
#define DEBUG

#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) \
    if (!(x))     \
    __debugbreak();

#ifdef DEBUG
#define GL(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GL(x) x;
#endif

#define LOG(x) std::cout << (x) << std::endl

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);