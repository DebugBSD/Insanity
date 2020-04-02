/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2020, DebugBSD
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "GameApplication.h"

GameApplication::GameApplication():
    m_Height{0},
    m_Width{0},
    m_pWindow{nullptr},
    m_Projection{0}
{
}

bool GameApplication::Init(int width, int height, const std::string& windowTitle)
{
    m_Height = height;
    m_Width = width;

    if (glfwInit() == GLFW_FALSE)
    {
        // TODO: Handle error.
        std::cout << "ERROR: GLFW initialization failed." << std::endl;
        glfwTerminate();
        return false;
    }

    // Setup GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_pWindow = glfwCreateWindow(m_Width, m_Height, "Test OpenGL Windows", nullptr, nullptr);
    if (m_pWindow == nullptr)
    {
        // TODO: Handle error.
        std::cout << "ERROR: GLFW window creation failed." << std::endl;
        glfwTerminate();
        return false;
    }

    // Get buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(m_pWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW
    glfwMakeContextCurrent(m_pWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        // TODO: Handle Errors 
        std::cout << "ERROR: " << glewGetErrorString(res) << std::endl;
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
        return false;
    }

    if (!GLEW_EXT_framebuffer_object)
    {
        // TODO: Handle error.
        std::cout << "ERROR: There is no Extension: GLEW_EXT_framebuffer_object" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, bufferWidth, bufferHeight);

    CreateTriangle();
    CreateShaders();

    m_Projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 1000.0f);

    return true;
}

void GameApplication::Run()
{
    while (!glfwWindowShouldClose(m_pWindow))
    {
        // Detect any external event (Mouse, Keyboard, ...)
        glfwPollEvents();

        // Clear the Window
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Here we render the scene.
        m_ShaderList[0]->UseShader();
        GLuint uniformModel = m_ShaderList[0]->GetModelLocation();
        GLuint uniformProjection = m_ShaderList[0]->GetProjectionLocation();

        // Renderizamos el primer objeto
        // Aplicamos Transform
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        // Establecemos el valor al shader.
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(m_Projection));

        m_MeshList[0]->RenderMesh();

        // Renderizamos el segundo objeto.
        // Aplicamos el transform
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        m_MeshList[1]->RenderMesh();

        glUseProgram(0);

        // Draw the scene.
        glfwSwapBuffers(m_pWindow);
    }
}

void GameApplication::Shutdown()
{
    glfwTerminate();
}

void GameApplication::ProcessInput()
{
}

void GameApplication::Update(float dt)
{
}

void GameApplication::Render()
{
}

// VAO will hold multiple VBO
void GameApplication::CreateTriangle()
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    Mesh* pMesh = new Mesh();
    pMesh->CreateMesh(vertices, indices, 12, 12);
    m_MeshList.push_back(pMesh);

    pMesh = new Mesh();
    pMesh->CreateMesh(vertices, indices, 12, 12);
    m_MeshList.push_back(pMesh);
}

void GameApplication::CreateShaders()
{
    // Vertex Shader
    static const char* vShader = "../Resources/Shaders/vShader.vert";

    // Fragment Shader
    static const char* fShader = "../Resources/Shaders/fShader.frag";

    Shader* pShader = new Shader();
    pShader->CreateFromFile(vShader, fShader);
    m_ShaderList.push_back(pShader);
}