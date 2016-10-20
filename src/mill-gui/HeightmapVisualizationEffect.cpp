#include "HeightmapVisualizationEffect.hpp"
#include "OpenGLHeaders.hpp"
#include "Config.hpp"

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
