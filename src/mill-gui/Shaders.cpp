#include "Shaders.hpp"
#include <cstdio>
#include <iostream>

using namespace std;

Shader::Shader() : _shaderType(0), _shaderId(0) {
}

Shader::~Shader() {
  if (!_shaderId) {
    glDeleteShader(_shaderId);
    _shaderId = 0;
  }
}

void Shader::loadShaderFromFile(GLenum shaderType, string filename) {
  FILE *f = fopen(filename.c_str(), "rb");
  if (!f) {
    cerr << "Error: Cannot load shader: \"" << filename << "\"." << endl;
    return;
  }

  fseek(f, 0, SEEK_END);
  auto size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *shaderCode = new char[size+1];
  fread(shaderCode, size, 1, f);
  shaderCode[size] = 0;
  fclose(f);

  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderCode, nullptr); 
  glCompileShader(shader);
  delete[] shaderCode;

  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cout << "Error: Shader compilation failed" << std::endl 
      << infoLog << std::endl;
    return; // todo: throw exception
  }

  _shaderType = shaderType;
  _shaderId = shader;
}

VertexShader::VertexShader(const char *filename) {
  loadShaderFromFile(GL_VERTEX_SHADER, filename);
}

VertexShader::~VertexShader() {
}

FragmentShader::FragmentShader(const char *filename) {
  loadShaderFromFile(GL_FRAGMENT_SHADER, filename);
}

FragmentShader::~FragmentShader() {
}

ShaderProgram::ShaderProgram() {
  _program = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
  if (_program) {
    glDeleteProgram(_program);
    _program = 0;
  }
}

void ShaderProgram::attach(Shader *shader) {
  glAttachShader(_program, shader->getId());
}

void ShaderProgram::link() {
  glLinkProgram(_program);

  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(_program, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(_program, 512, NULL, infoLog);
      std::cout << "Error: Shader link" << std::endl << infoLog << std::endl;
  }
}

void ShaderProgram::use()
{
    glUseProgram(_program);
}
