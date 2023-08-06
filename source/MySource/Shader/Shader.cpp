#include "Shader.h"
#include "../Debug/Debug.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>
#include <sstream>


void Shader::CheckCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

ShaderPtr Shader::Create(const std::string& filenameVS, const std::string& filenameFS,const char* filenameGS)
{
	return std::make_shared<Shader>(filenameVS, filenameFS,filenameGS);
}

Shader::Shader(const std::string& filenameVS, const std::string& filenameFS,
    const char* filenameGS)
	: filenameVS(filenameVS), filenameFS(filenameFS),filenameGS(filenameGS)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(filenameVS);
        fShaderFile.open(filenameFS);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (filenameGS != "")
        {
            gShaderFile.open(filenameGS);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShaderCode, NULL);
    glCompileShader(vs);
    CheckCompileErrors(vs, "VERTEX");

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fShaderCode, NULL);
    glCompileShader(fs);
    CheckCompileErrors(fs, "FRAGMENT");

    if (filenameGS != "")
    {
        const char* gShaderCode = geometryCode.c_str();
        gs = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gs, 1, &gShaderCode, NULL);
        glCompileShader(gs);
        CheckCompileErrors(gs, "GEOMETRY");
    }
    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    if (filenameGS != "")
        glAttachShader(id, gs);
    glLinkProgram(id);
    CheckCompileErrors(id, "PROGRAM");
}

Shader::~Shader()
{
	glDeleteProgram(id);
	glDeleteShader(vs);
	glDeleteShader(fs);
    glDeleteShader(gs);
}

void Shader::Use() const noexcept
{
	glUseProgram(id);
}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value1, float value2) const
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), value1, value2);
}

void Shader::SetFloat(const std::string& name, glm::vec2& value) const
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
}

void Shader::SetFloat(const std::string& name, float value1, float value2, float value3) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), value1, value2, value3);
}

void Shader::SetFloat(const std::string& name, glm::vec3& value) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}

void Shader::SetFloat(const std::string& name, float value1, float value2, float value3, float value4) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), value1, value2, value3, value4);
}

void Shader::SetFloat(const std::string& name, glm::vec4& value) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::UniformBlockBinding(const std::string& name)
{
    if (name == "")return;
    unsigned int index = glGetUniformBlockIndex(id, name.c_str());
    glUniformBlockBinding(id,index,0);
}
