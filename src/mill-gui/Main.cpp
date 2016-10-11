#include "ImGuiBinding.hpp"
#include "Shaders.hpp"
#include "Config.hpp"
#include "TextureUtils.hpp"
#include "DebugShapes.hpp"
#include "HeightmapGeometry.hpp"
#include "MillerApplication.hpp"

using namespace std;

int main(void)
{
    MillerApplication millerApp;
    millerApp.create();
    millerApp.run();
    return 0;
}
