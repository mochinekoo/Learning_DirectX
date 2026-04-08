#include <Windows.h>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

int initWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

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

    ShowWindow(hwnd, nShowCmd); //ウインドウを表示
    UpdateWindow(hwnd);

    //メッセージ
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}