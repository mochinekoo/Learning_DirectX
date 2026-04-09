#include "Input.h"

void Input::initKey(HINSTANCE hinstance, HWND hwnd) {
	HRESULT hr;
	DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDI, NULL);
	hr = g_pDI->CreateDevice(GUID_SysKeyboard, &g_pKeyboard, NULL);
	if (FAILED(hr)) {
		MessageBox(hwnd, L"初期化失敗したお", L"タイトル", MB_OK);
	}

	hr = g_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) {
		MessageBox(hwnd, L"初期化失敗したお", L"タイトル", MB_OK);
	}
	hr =g_pKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) {
		MessageBox(hwnd, L"初期化失敗したお", L"タイトル", MB_OK);
	}
}

void Input::SetInputState() {
	HRESULT hr = g_pKeyboard->GetDeviceState(sizeof(keys), keys);

	if (FAILED(hr)) {
		g_pKeyboard->Acquire();
	}
}

bool Input::GetInputState(int key)
{
	if (Input::keys[key] & 0x80) return true;
	return false;
}

