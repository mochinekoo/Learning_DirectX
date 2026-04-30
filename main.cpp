#include <Windows.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d2d1.h>
#include <dwrite.h>
#include "dinput.h"
#include "input.h"
#include <cstdio>
#include "MyDirectX.h"
#include "Triangle.h"
using namespace MyDirectX;
#pragma comment(lib , "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d2d1.lib") 
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace {
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    Triangle* triangle = nullptr;
    Triangle* triangle2 = nullptr;
}

int CreateTextDevice();
void onRender();
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
    triangle = new Triangle(
        -0.5f, 0.5f,
        0.5f, 0.5f,
        0.5f, -0.5f);
    triangle->Initialize();

    triangle2 = new Triangle(
        -1.0f, 0.5f,
        -0.6f, 0.5f,
        -0.6f, -0.5f);
    triangle2->Initialize();
   
    ShowWindow(hwnd, nShowCmd); //ウインドウを表示
    UpdateWindow(hwnd);

    Input::initKey(hInstance, hwnd);

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
            //onRender();
            triangle->Update();
            triangle2->Update();
            triangle->Draw();
            triangle2->Draw();

            MyDirectX::swapChain->Present(1, 0);

            Input::SetInputState();
            if (Input::GetInputState(DIK_W)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                break;
            }
        }
    }

    return 0;
}

int initDirectX(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Width = WINDOW_WIDTH; //画面の幅
    desc.BufferDesc.Height = WINDOW_HEIGHT; //画面の幅
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //各8ビットずつの色
    desc.Windowed = TRUE; //ウインドウモード
    desc.OutputWindow = hwnd; //出力するウインドウ
    desc.BufferCount = 1; //バファー（裏画面）の数
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SampleDesc.Count = 4; //サンプル数
    desc.SampleDesc.Quality = 0; //品質レベル
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;

    HRESULT hresult = {};
    D3D_FEATURE_LEVEL level = {};
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    hresult = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
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


    hresult = MyDirectX::device_->CreateRenderTargetView(MyDirectX::texture2D, nullptr, &MyDirectX::renderTargetView);
    MyDirectX::texture2D->Release(); //メモリーを開放する

    assert(SUCCEEDED(hresult));

    CreateTextDevice();

    IDXGISurface* pBackBuffer = nullptr;
    hresult = MyDirectX::swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    assert(SUCCEEDED(hresult));

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );

    hresult = MyDirectX::d2dFactory->CreateDxgiSurfaceRenderTarget(
        pBackBuffer,
        &props,
        &MyDirectX::d2dRenderTarget
    );
    assert(SUCCEEDED(hresult));

    pBackBuffer->Release();

    hresult = MyDirectX::d2dRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &MyDirectX::brush
    );
    assert(SUCCEEDED(hresult));

    D3D11_VIEWPORT viewport = {};
    viewport.Width = WINDOW_WIDTH;
    viewport.Height = WINDOW_HEIGHT;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    context_->RSSetViewports(1, &viewport);


    MyDirectX::initialize();
    
    return 0;
}

int CreateTextDevice() {
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 50;
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &MyDirectX::d2dFactory);

    if (SUCCEEDED(hr))
    {
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(MyDirectX::dwriteFactory),
            reinterpret_cast<IUnknown**>(&MyDirectX::dwriteFactory)
        );
    }
    if (SUCCEEDED(hr))
    {

        hr = MyDirectX::dwriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &MyDirectX::textFormat
        );
    }
    if (SUCCEEDED(hr))
    {
        MyDirectX::textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

        MyDirectX::textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }



    return hr;
}

void onRender() {
    HRESULT hr;
    static const WCHAR sc_helloWorld[] = L"Hello, World!";

    D2D1_SIZE_F renderTargetSize = MyDirectX::d2dRenderTarget->GetSize();

    MyDirectX::d2dRenderTarget->BeginDraw();

    MyDirectX::d2dRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    MyDirectX::d2dRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    MyDirectX::d2dRenderTarget->DrawText(
        sc_helloWorld,
        ARRAYSIZE(sc_helloWorld) - 1,
        MyDirectX::textFormat,
        D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
        MyDirectX::brush
    );

    hr = MyDirectX::d2dRenderTarget->EndDraw();

    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
    }
}