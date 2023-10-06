#include <memory>
#include "FrameWork.h"
#include "GameSystem.h"

using namespace Microsoft::WRL;
using BlendMode = HelperGraphicPipeline::BlendMode;

// Windowsアプリでのエントリーポイント(main関数)
int32_t WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int32_t) {

    std::unique_ptr<FrameWork> game = std::make_unique<GameSystem>();
    game->Run();

    return 0;
}
