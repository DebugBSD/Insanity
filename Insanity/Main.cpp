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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "TArray.h"

const GLint HEIGHT = 768, WIDTH = 1024;
GLuint VAO, VBO, shader, uniformModel;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

// Vertex Shader
static const char* vShader = "../Resources/Shaders/vShader.vert";

// Fragment Shader
static const char* fShader = "../Resources/Shaders/fShader.frag";


// VAO will hold multiple VBO
void CreateTriangle()
{
    GLfloat vertices[] =
    {
        //X     Y     Z
        -1.0, -1.0,  0.0,
         1.0, -1.0,  0.0,
         0.0,  1.0,  0.0
    };

    // Creamos un Vertex Array Object. Este VAO contiene todos los VBO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Creamos un Vertex Buffer Object. Este VBO contiene el Mesh 3D.
    // Puede contener ademas, otros tipos de elementos.
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void AddShader(GLuint program, const char* shaderCode, GLenum shaderType)
{
    GLint errorCode = 0;
    GLchar buffer[1024];
    const GLchar* pCode[1];

    std::ifstream shaderFile{ shaderCode };
    if (shaderFile.is_open())
    {
        std::stringstream sstream;
        sstream << shaderFile.rdbuf();
        std::string contents = sstream.str();

        pCode[0] = contents.c_str();

        GLint codeLength[1];
        codeLength[0] = strlen(contents.c_str());

        GLuint currentShader = glCreateShader(shaderType);

        glShaderSource(currentShader, 1, pCode, codeLength);
        glCompileShader(currentShader);

        glGetShaderiv(currentShader, GL_COMPILE_STATUS, &errorCode);
        if (!errorCode)
        {
            glGetShaderInfoLog(currentShader, sizeof(buffer), nullptr, buffer);
            std::cout << "ERROR (COMPILER (" << shaderType << ")): " << buffer << std::endl;
            return;
        }

        glAttachShader(program, currentShader);
    }
    else
    {
        // TODO: Handle error
    }
}

bool CompileShader(const char *vShader, const char *fShader)
{
    shader = glCreateProgram();

    if (!shader)
    {
        // TODO: Handle error.
        std::cout << "ERROR: Creating shader." << std::endl;
        return false;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);

    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint errorCode = 0;
    GLchar buffer[1024];

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &errorCode);
    if (!errorCode)
    {
        glGetProgramInfoLog(shader, sizeof(buffer), nullptr, buffer);
        std::cout << "ERROR (LINKER): " << buffer << std::endl;
        return false;
    }

    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &errorCode);
    if (!errorCode)
    {
        glGetProgramInfoLog(shader, sizeof(buffer), nullptr, buffer);
        std::cout << "ERROR (VALIDATE): " << buffer << std::endl;
        return false;
    }

    // Cogemos el valor de la variable uniform declarada en el shader.
    uniformModel = glGetUniformLocation(shader, "model");

    return true;
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

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

    glViewport(0,0,bufferWidth, bufferHeight);

    CreateTriangle();
    CompileShader(vShader, fShader);

    while (!glfwWindowShouldClose(pWindow))
    {
        // Detect any external event (Mouse, Keyboard, ...)
        glfwPollEvents();

        if (direction == true)
        {
            triOffset += triIncrement;

        }
        else
        {
            triOffset -= triIncrement;
        }

        if (abs(triOffset) >= triMaxOffset)
        {
            direction = !direction;
        }

        // Clear the Window
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Here we render the scene.
        glUseProgram(shader);

        // Transform
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(triOffset, 0,0));

        // Establecemos el valor al shader.
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glUseProgram(0);

        // Draw the scene.
        glfwSwapBuffers(pWindow);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}

