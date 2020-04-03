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
#include "CameraComponent.h"

GameApplication::GameApplication():
    m_Height{0},
    m_Width{0},
    m_pWindow{nullptr},
    m_Projection{0},
    m_LastX{ 0 },
    m_LastY{ 0 },
    m_XChange{ 0 },
    m_YChange{ 0 },
    m_MouseFirstMoved{ true },
    m_pCameraComponent{nullptr}, 
    deltaTime{0.0f},
    lastTime{0.0f}
{
}

bool GameApplication::Init(int width, int height, const std::string& windowTitle)
{
    m_Height = height;
    m_Width = width;

    for (auto& key : m_Keys)
    {
        key = false;
    }

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
    glfwGetFramebufferSize(m_pWindow, &m_BufferWidth, &m_BufferHeight);

    // Set context for GLEW
    glfwMakeContextCurrent(m_pWindow);

    CreateCallbacks();

    // Desactivamos el cursor (al estilo de un First Person Shooter)
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    glViewport(0, 0, m_BufferWidth, m_BufferHeight);

    // We setup our input handler.
    glfwSetWindowUserPointer(m_pWindow, this);

    
    return true;
}

void GameApplication::Run()
{
    // We set our scene

    CreateTriangle();
    CreateShaders();

    m_pCameraComponent = new CameraComponent(
        glm::vec3{ 0.0f, 0.5f, -3.5f }, 
        glm::vec3{ 0.0f, 1.0f, 0.0f }, 
        -90.0f,
        0.0f, 
        5.0f,
        1.0f);

    m_Projection = glm::perspective(45.0f, (GLfloat)m_BufferWidth / (GLfloat)m_BufferHeight, 0.1f, 1000.0f);

    GLuint uniformModel{ 0 }, uniformProjection{ 0 }, uniformView{ 0 };
    while (!glfwWindowShouldClose(m_pWindow))
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;
        // Detect any external event (Mouse, Keyboard, ...)
        glfwPollEvents();

        m_pCameraComponent->KeyControl(m_Keys, deltaTime);
        m_pCameraComponent->MouseControl(GetXChange() , GetYChange());

        // Clear the Window
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Here we render the scene.
        m_ShaderList[0]->UseShader();
        uniformModel = m_ShaderList[0]->GetModelLocation();
        uniformProjection = m_ShaderList[0]->GetProjectionLocation();
        uniformView = m_ShaderList[0]->GetViewLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(m_Projection));


        // Renderizamos el primer objeto
        // Aplicamos Transform
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        // Establecemos el valor al shader.
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(m_pCameraComponent->CalculateViewMatrix()));
        m_MeshList[0]->RenderMesh();

        // Renderizamos el segundo objeto.
        // Aplicamos el transform
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(m_pCameraComponent->CalculateViewMatrix()));
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

GLfloat GameApplication::GetXChange()
{
    GLfloat x = m_XChange;
    m_XChange = 0.0f;
    return x;
}

GLfloat GameApplication::GetYChange()
{
    GLfloat y = m_YChange;
    m_YChange = 0.0f;
    return y;
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

void GameApplication::CreateCallbacks()
{
    glfwSetKeyCallback(m_pWindow, handKeys); 
    glfwSetCursorPosCallback(m_pWindow, handMouse);
}

void GameApplication::handKeys(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    GameApplication* pMainWindow = static_cast<GameApplication*>(glfwGetWindowUserPointer(pWindow));
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            pMainWindow->m_Keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            pMainWindow->m_Keys[key] = false;
        }
    }
}

void GameApplication::handMouse(GLFWwindow* pWindow, double xPos, double yPos)
{
    GameApplication* pMainWindow = static_cast<GameApplication*>(glfwGetWindowUserPointer(pWindow));

    if (pMainWindow->m_MouseFirstMoved)
    {
        pMainWindow->m_LastX = xPos;
        pMainWindow->m_LastY = yPos;
        pMainWindow->m_MouseFirstMoved = false;
    }

    pMainWindow->m_XChange = xPos - pMainWindow->m_LastX;
    pMainWindow->m_YChange = pMainWindow->m_LastY - yPos;

    pMainWindow->m_LastX = xPos;
    pMainWindow->m_LastY = yPos;
}
