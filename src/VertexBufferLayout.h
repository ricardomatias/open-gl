#pragma once

#include <vector>
#include <GL/glew.h>

#include "ErrorHandler.h"

template <typename T>
struct FAIL_ASSERT : std::false_type
{
};

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type)
        {
        case GL_FLOAT:
            return 4;
            break;
        
        case GL_UNSIGNED_INT:
            return 4;
            break;
        
        case GL_UNSIGNED_BYTE:
            return 4;
            break;
        default:
            ASSERT(false);
            return 0;
            break;
        }
    }
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;

public:
    VertexBufferLayout(/* args */)
        : m_Stride(0) {};

    template <typename T>
    void Push(unsigned int count)
    {
        static_assert(FAIL_ASSERT<T>::value); // in case we don't know the type (default-like)
    }

    inline const std::vector<VertexBufferElement> &GetElements() const { return m_Elements; };
    inline unsigned int GetStride() const { return m_Stride; };
};

template <>
inline void VertexBufferLayout::Push<float>(unsigned int count)
{
    m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
}

template <>
inline void VertexBufferLayout::Push<unsigned int>(unsigned int count)
{
    m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
}

template <>
inline void VertexBufferLayout::Push<unsigned char>(unsigned int count)
{
    m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
}

