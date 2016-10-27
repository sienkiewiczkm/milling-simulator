#include "HeightmapVisualizationEffect.hpp"
#include "OpenGLHeaders.hpp"
#include "Config.hpp"

#include <glm/gtc/type_ptr.hpp>

using namespace std;

namespace ms
{

HeightmapVisualizationEffect::HeightmapVisualizationEffect()
{
}

HeightmapVisualizationEffect::~HeightmapVisualizationEffect()
{
}

void HeightmapVisualizationEffect::create()
{
    createShaders();
}

void HeightmapVisualizationEffect::destroy()
{
}

void HeightmapVisualizationEffect::begin()
{
    _shaderProgram->use();
}

void HeightmapVisualizationEffect::end()
{
}

void HeightmapVisualizationEffect::setTextureMatrix(
    const glm::mat4 &textureMatrix
)
{
    _textureMatrix = textureMatrix;

    GLint uniformLoc = glGetUniformLocation(
        _shaderProgram->getId(),
        "TextureMatrix"
    );

    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(_textureMatrix));
}

void HeightmapVisualizationEffect::setAlbedoTexture(GLuint textureId)
{
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(
        glGetUniformLocation(_shaderProgram->getId(), "AlbedoTexture"),
        0
    );
}

void HeightmapVisualizationEffect::setHeightmapTexture(GLuint textureId)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(
        glGetUniformLocation(_shaderProgram->getId(), "HeightmapTexture"),
        1
    );
}

void HeightmapVisualizationEffect::setSize(const glm::vec3 &size)
{
    glUniform3fv(
        glGetUniformLocation(_shaderProgram->getId(), "HeightmapSize"),
        1,
        value_ptr(size)
    );
}

void HeightmapVisualizationEffect::createShaders()
{
    shared_ptr<Shader> vs = make_shared<Shader>();
    vs->addSourceFromFile(RESOURCE("shaders/HeightmapTransform.vert"));
    vs->compile(GL_VERTEX_SHADER);

    shared_ptr<Shader> fs = make_shared<Shader>();
    fs->addSourceFromFile(RESOURCE("shaders/HeightmapTransform.frag"));
    fs->compile(GL_FRAGMENT_SHADER);

    _shaderProgram = make_shared<ShaderProgram>();
    _shaderProgram->attach(vs.get());
    _shaderProgram->attach(fs.get());
    _shaderProgram->link();
}

}
