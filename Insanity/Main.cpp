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

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "TArray.h"
#include "Mesh.h"
#include "Shader.h"

const GLint HEIGHT = 768, WIDTH = 1024;
const float toRadians = 3.14159265f / 180.0f;

// Vertex Shader
static const char* vShader = "../Resources/Shaders/vShader.vert";

// Fragment Shader
static const char* fShader = "../Resources/Shaders/fShader.frag";

std::vector<Mesh*> m_MeshList;
std::vector<Shader*> m_ShaderList;

// VAO will hold multiple VBO
void CreateTriangle()
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

void CreateShaders()
{
    Shader* pShader = new Shader();
    pShader->CreateFromFile(vShader, fShader);
    m_ShaderList.push_back(pShader);
}

int main()
{
    TArray<int> arrayOfInt{ 10 };
    arrayOfInt.Append(15,15);

    for (int i = 0; i < 10; i++)
    {
        arrayOfInt[i] = i;
    }

    if (glfwInit() == GLFW_FALSE)
    {
        // TODO: Handle error.
        std::cout << "ERROR: GLFW initialization failed." << std::endl;
        glfwTerminate();
        return -3;
    }

    // Setup GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test OpenGL Windows", nullptr, nullptr);
    if (pWindow == nullptr)
    {
        // TODO: Handle error.
        std::cout << "ERROR: GLFW window creation failed." << std::endl;
        glfwTerminate();
        return -4;
    }

    // Get buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(pWindow, &bufferWidth, &bufferHeight);
    
    // Set context for GLEW
    glfwMakeContextCurrent(pWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        // TODO: Handle Errors 
        std::cout << "ERROR: " << glewGetErrorString(res) << std::endl;
        glfwDestroyWindow(pWindow);
        glfwTerminate();
        return -1;
    }

    if (!GLEW_EXT_framebuffer_object)
    {
        // TODO: Handle error.
        std::cout << "ERROR: There is no Extension: GLEW_EXT_framebuffer_object" << std::endl;
        return -2;
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0,0,bufferWidth, bufferHeight);

    CreateTriangle();
    CreateShaders();

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth/(GLfloat)bufferHeight, 0.1f, 1000.0f);

    while (!glfwWindowShouldClose(pWindow))
    {
        // Detect any external event (Mouse, Keyboard, ...)
        glfwPollEvents();

        // Clear the Window
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Here we render the scene.
        m_ShaderList[0]->UseShader();
        GLuint uniformModel = m_ShaderList[0]->GetModelLocation();
        GLuint uniformProjection = m_ShaderList[0]->GetProjectionLocation();

        // Renderizamos el primer objeto
        // Aplicamos Transform
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f ,-2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        // Establecemos el valor al shader.
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

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
        glfwSwapBuffers(pWindow);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}

