#include "TexturedPhongEffect.hpp"

#include <glm/gtc/type_ptr.hpp>

using namespace std;

TexturedPhongEffect::TexturedPhongEffect()
{
}

TexturedPhongEffect::~TexturedPhongEffect()
{
}

void TexturedPhongEffect::create()
{
    createShaders();
}

void TexturedPhongEffect::destroy()
{
}

void TexturedPhongEffect::begin()
{
    _program->use();
}

void TexturedPhongEffect::end()
{
}

void TexturedPhongEffect::setModelMatrix(const glm::mat4 &modelMatrix)
{
    GLint modelLoc = glGetUniformLocation(_program->getId(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void TexturedPhongEffect::setViewMatrix(const glm::mat4 &viewMatrix)
{
    GLint viewLoc = glGetUniformLocation(_program->getId(), "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

void TexturedPhongEffect::setProjectionMatrix(const glm::mat4 &projMatrix)
{
    GLint projLoc = glGetUniformLocation(_program->getId(), "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
}

void TexturedPhongEffect::setTexture(GLuint textureId)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(glGetUniformLocation(_program->getId(), "TextureSlot1"), 0);
}

void TexturedPhongEffect::createShaders()
{
    shared_ptr<Shader> vs = make_shared<Shader>();
    vs->addSourceFromFile(RESOURCE("shaders/MVPTransformTexCoord.vert"));
    vs->compile(GL_VERTEX_SHADER);

    shared_ptr<Shader> fs = make_shared<Shader>();
    fs->addSourceFromFile(RESOURCE("shaders/SingleTexture.frag"));
    fs->compile(GL_FRAGMENT_SHADER);

    _program = make_shared<ShaderProgram>();
    _program->attach(vs.get());
    _program->attach(fs.get());
    _program->link();
}
