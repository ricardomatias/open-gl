#pragma once
#define DEBUG

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer
{
private:
    /* data */
public:
    Renderer(/* args */);
    ~Renderer();

    void Clear() const;
    void Draw(const VertexArray&, const IndexBuffer&, const Shader&) const;
};

