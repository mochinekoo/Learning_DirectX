#include <Windows.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace {
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
}

namespace MyDirectX {
    ID3D11Device* device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11Texture2D* texture2D = nullptr;
}

int initWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
int initDirectX(HWND hwnd);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    if (initWindow(hInstance, hPrevInstance, lpCmdLine, nShowCmd) == -1) {
        return -1;
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY: {
        PostQuitMessage(0); //メッセージループ終了（=アプリを終了）
        break;
    }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


int initWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    WNDCLASSEXW wndClass = {};
    wndClass.cbSize = sizeof(WNDCLASSEXW);
    wndClass.lpszClassName = L"WindowClass";
    wndClass.lpfnWndProc = WndProc;
    wndClass.style = CS_VREDRAW | CS_HREDRAW;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    RegisterClassEx(&wndClass);

    //ウインドウを作成する
    HWND hwnd = {};
    hwnd = CreateWindow(
        L"WindowClass", //クラス名
        L"テストウインドウ", //ウインドウタイトル
        WS_BORDER | WS_OVERLAPPEDWINDOW, //ウインドウスタイル
        CW_USEDEFAULT, //表示位置X（デフォルト）
        CW_USEDEFAULT, //表示位置Y（デフォルト）
        WINDOW_WIDTH, //ウインドウ幅
        WINDOW_HEIGHT, //ウインドウ高さ
        NULL, //親ウインドウ
        NULL, //メニュー
        hInstance, //インスタンス
        NULL //パラメータ
    );

    if (hwnd == NULL) {
        return -1;
    }

    if (initDirectX(hwnd)) {
        return -1;
    }

    ShowWindow(hwnd, nShowCmd); //ウインドウを表示
    UpdateWindow(hwnd);

    //メッセージ
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            float color[4] = { 0, 0, 0, 1.0f };

            MyDirectX::context_->OMSetRenderTargets(1, &MyDirectX::renderTargetView, nullptr);
            MyDirectX::context_->ClearRenderTargetView(MyDirectX::renderTargetView, color);
            MyDirectX::swapChain->Present(1, 0);
        }
    }

    return 0;
}

int initDirectX(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Width = WINDOW_WIDTH;
    desc.BufferDesc.Height = WINDOW_HEIGHT;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Windowed = TRUE;
    desc.OutputWindow = hwnd;
    desc.BufferCount = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;

    HRESULT hresult = {};
    D3D_FEATURE_LEVEL level = {};
    hresult = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &desc,
        &MyDirectX::swapChain,
        &MyDirectX::device_,
        &level,
        &MyDirectX::context_
    );

    if (FAILED(hresult)) {
        MessageBox(hwnd, L"DirectXの初期化に失敗したよ", NULL, MB_OK);
        return -1;
    }

    MyDirectX::swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&MyDirectX::texture2D);


    MyDirectX::device_->CreateRenderTargetView(MyDirectX::texture2D, nullptr, &MyDirectX::renderTargetView);
    MyDirectX::texture2D->Release();

    if (FAILED(hresult)) {
        MessageBox(hwnd, L"DirectXの初期化に失敗したよ", NULL, MB_OK);
        return -1;
    }
    
    return 0;
}