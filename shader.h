#ifndef SHADER_H
#define SHADER_H
// GLEW
#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//************************************************************************************************************************************************
void shaderBuildProgram(const GLchar* vertexPath, const GLchar* fragmentPath, GLuint* Program)
{
    // Получаем исходный код шейдера из filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    if (vShaderFile.is_open()&& fShaderFile.is_open())
    {
        std::stringstream vShaderStream, fShaderStream;
                                                            // Считываем данные в потоки
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
                                                            // Закрываем файлы
        vShaderFile.close();
        fShaderFile.close();
                                                            // Преобразовываем потоки в массив GLchar
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    else
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
   
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    // Сборка шейдеров
    GLuint vertex, fragment;
    GLint success;
    const int sizeInfoLog = 512;
    GLchar infoLog[sizeInfoLog];
                                                                        // Вершинный шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);                 // Если есть ошибки - вывести их
    if (!success)
    {
        glGetShaderInfoLog(vertex, sizeInfoLog, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
                                                                        // Фрагментный шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);               // Если есть ошибки - вывести их
    if (!success)
    {
        glGetShaderInfoLog(fragment, sizeInfoLog, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    

    *Program = glCreateProgram();                                       // Шейдерная программа
    glAttachShader(*Program, vertex);
    glAttachShader(*Program, fragment);
    glLinkProgram(*Program);
    
    glGetProgramiv(*Program, GL_LINK_STATUS, &success);                 // Если есть ошибки - вывести их
    if (!success)
    {
        glGetProgramInfoLog(*Program, sizeInfoLog, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Удаляем шейдеры, поскольку они больше не нужны.
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

//************************************************************************************************************************************************

void Use(GLuint program)
{
    glUseProgram(program);
}

//************************************************************************************************************************************************
#endif