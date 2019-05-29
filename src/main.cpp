#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ErrorHandler.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"


int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    GLenum err = glewInit();

    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    fprintf(stdout, "Status: Usindg GLEW %s\n", glewGetString(GLEW_VERSION));

    std::cout << glGetString(GL_VERSION) << std::endl;

    // DRAWING ONE TRIANGLE
    {
        float positions[]{
            -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); // 1 - src.a;

        VertexArray va;
        va.Bind();
        
        int fPerVertex = 4;
        VertexBuffer vb(positions, 4 * fPerVertex * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        va.AddBuffer(vb, layout);

        IndexBuffer ibo(indices, 6);

        std::string shaderPath = "res/shaders/basic.shader";
        Shader shader(shaderPath);

        shader.Bind();
        shader.SetUniformMat4f("u_MVP", proj);

        std::string texturePath = "res/textures/cherno.png";
        Texture texture(texturePath);

        texture.Bind();
        shader.SetUniform1i("u_Texture", 0); // because our texture slot is bound to slot 0

        float r = 0.0f;
        auto t_start = std::chrono::high_resolution_clock::now();

        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ibo.Unbind();

        Renderer renderer;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            auto t_now = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

            r = (1.f + glm::sin(time)) * 0.5f;

            renderer.Clear();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.0f, 1.0f, 1.0f); // because our texture slot is bound to slot 0

            /* Draw call */
            renderer.Draw(va, ibo, shader);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
