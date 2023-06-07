#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<math.h>
using namespace DirectX;
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<Windows.h>
#include<vector>
#include<string>
#include<DirectXTex.h>
#include<wrl.h>
#include "Util.h"
#include <memory>

#include "WndAPI.h"
#include "Input.h"
#include "InitDirectX.h"
#include "TextureManager.h"
#include "ConstBuffer.h"
#include "GraphicsPipeline.h"
#include "CameraManager.h"
#include "Sprite.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SceneManager.h"

#include "FrameWork.h"
#include "GameSystem.h"

using namespace Microsoft::WRL;
using BlendMode = HelperGraphicPipeline::BlendMode;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::unique_ptr<FrameWork> game = std::make_unique<GameSystem>();
    game->Run();

    return 0;
}
