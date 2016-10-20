#include "TexturedPhongEffect.hpp"
#include "Config.hpp"

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
    _shaderProgram->use();
}

void TexturedPhongEffect::end()
{
}

void TexturedPhongEffect::setTexture(GLuint textureId)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(
        glGetUniformLocation(_shaderProgram->getId(), "TextureSlot1"),
        0
    );
}

void TexturedPhongEffect::createShaders()
{
    shared_ptr<Shader> vs = make_shared<Shader>();
    vs->addSourceFromFile(RESOURCE("shaders/MVPTransformTexCoord.vert"));
    vs->compile(GL_VERTEX_SHADER);

    shared_ptr<Shader> fs = make_shared<Shader>();
    fs->addSourceFromFile(RESOURCE("shaders/SingleTexture.frag"));
    fs->compile(GL_FRAGMENT_SHADER);

    _shaderProgram = make_shared<ShaderProgram>();
    _shaderProgram->attach(vs.get());
    _shaderProgram->attach(fs.get());
    _shaderProgram->link();
}
