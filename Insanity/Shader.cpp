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

#include "Shader.h"


Shader::Shader():
    m_ShaderID{0},
    m_UniformModel{0},
    m_UniformProjection{0}
{
}

Shader::~Shader()
{
    ClearShader();
}

void Shader::CompileShader(const std::string& vCode, const std::string& fCode)
{
    m_ShaderID = glCreateProgram();

    if (!m_ShaderID)
    {
        // TODO: Handle error.
        std::cout << "ERROR: Creating shader." << std::endl;
        return;
    }

    AddShader(vCode, GL_VERTEX_SHADER);

    AddShader(fCode, GL_FRAGMENT_SHADER);

    GLint errorCode = 0;
    GLchar buffer[1024];

    glLinkProgram(m_ShaderID);
    glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &errorCode);
    if (!errorCode)
    {
        glGetProgramInfoLog(m_ShaderID, sizeof(buffer), nullptr, buffer);
        std::cout << "ERROR (LINKER): " << buffer << std::endl;
        return;
    }

    glValidateProgram(m_ShaderID);
    glGetProgramiv(m_ShaderID, GL_VALIDATE_STATUS, &errorCode);
    if (!errorCode)
    {
        glGetProgramInfoLog(m_ShaderID, sizeof(buffer), nullptr, buffer);
        std::cout << "ERROR (VALIDATE): " << buffer << std::endl;
        return;
    }

    // Cogemos el valor de la variable uniform declarada en el shader.
    m_UniformModel = glGetUniformLocation(m_ShaderID, "model");
    m_UniformProjection = glGetUniformLocation(m_ShaderID, "projection");
}

void Shader::AddShader(const std::string& shaderCode, GLenum shaderType)
{
    GLint errorCode = 0;
    GLchar buffer[1024];
    const GLchar* pCode[1];

    pCode[0] = shaderCode.c_str();

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode.c_str());

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

    glAttachShader(m_ShaderID, currentShader);
}

void Shader::CreateFromString(const std::string& vCode, const std::string& fCode)
{
    CompileShader(vCode, fCode);
}

void Shader::CreateFromFile(const std::string& vertexFile, const std::string& fragmentFile)
{
    std::ifstream vFile{ vertexFile };
    std::ifstream fFile{ fragmentFile };
    if (vFile.is_open() && fFile.is_open())
    {
        std::stringstream ss1, ss2;
        ss1 << vFile.rdbuf();
        ss2 << fFile.rdbuf();

        CreateFromString(ss1.str(), ss2.str());

    }
    else
    {
        // TODO: Handle error
    }
}

GLuint Shader::GetProjectionLocation()
{
    return m_UniformProjection;
}

GLuint Shader::GetModelLocation()
{
    return m_UniformModel;
}

void Shader::UseShader()
{
    glUseProgram(m_ShaderID);
}

void Shader::ClearShader()
{
    if (m_ShaderID)
    {
        glDeleteProgram(m_ShaderID);
        m_ShaderID = 0;
    }

    m_UniformProjection = 0;
    m_UniformModel = 0;
}
