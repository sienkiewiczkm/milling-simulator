#pragma once

#include <string>
#include <vector>

namespace fw
{
class IParametricSurfaceUV;
}

namespace ms
{

class CadioModelLoader
{
public:
    CadioModelLoader();
    ~CadioModelLoader();

    std::vector<std::shared_ptr<fw::IParametricSurfaceUV>> loadModel(
        const std::string path
    );
};

}
