#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <glad/glad.h>
#include <string>

class Shader {
public:
  Shader();
  Shader(const Shader &shader) = default;
  virtual ~Shader();

  GLenum getType() { return _shaderType; }
  GLuint getId() { return _shaderId; }

protected:
  void loadShaderFromFile(GLenum shaderType, std::string filename);

private:
  GLenum _shaderType;
  GLuint _shaderId;
};

class VertexShader : public Shader {
public:
  VertexShader(const char *filename);
  virtual ~VertexShader();
};

class FragmentShader : public Shader {
public:
  FragmentShader(const char *filename);
  virtual ~FragmentShader();
};

class ShaderProgram {
public:
  ShaderProgram();
  ~ShaderProgram();

  void attach(Shader *shader);
  void link();
  void use();
  GLuint getId() { return _program; }

private:
  GLuint _program;
};

#endif
