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

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "Shader.h"

class GameApplication
{
public:
    GameApplication();

    bool Init(int width, int height, const std::string &windowTitle);

    void Run();

    void Shutdown();

    bool* GetKeys() { return m_Keys; }

    GLfloat GetXChange();
    GLfloat GetYChange();

private:

    void ProcessInput();

    void Update(float dt);

    void Render();

    void CreateTriangle();

    void CreateShaders();

private:
    GLFWwindow* m_pWindow;
    // private members
    int m_Width;
    int m_Height;

    std::vector<Mesh*> m_MeshList;
    std::vector<Shader*> m_ShaderList;

    // Esto deberia estar en la clase Render
    glm::mat4 m_Projection;

    bool m_Keys[1024];
    GLfloat m_LastX;
    GLfloat m_LastY;
    GLfloat m_XChange;
    GLfloat m_YChange;
    bool m_MouseFirstMoved;

    // Handle key presses.
    void CreateCallbacks();
    static void handKeys(GLFWwindow* pWindow, int key, int scancode, int action, int mode);
    static void handMouse(GLFWwindow* pWindow, double xPos, double yPos);
};

