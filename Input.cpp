#include "Input.h"
#include <wrl.h>

using namespace Microsoft::WRL;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


Input* Input::GetInstance()
{
	static Input instance;

	return &instance;
}

void Input::Initialize(WinApp* winApp)
{
	HRESULT result;

	this->winApp = winApp;

	//DirectInputのインスタンス生成
	result = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr
	);

	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);


	//入力データ形式のセット
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);	//標準形式

	//排他的制御レベルのリセット
	result = devkeyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
}

void Input::Update()
{
	HRESULT result;

	//前回のキー入力を保存
	memcpy(Oldkey, key, sizeof(key));

	//キーボード情報の取得開始
	result = devkeyboard->Acquire();

	//全てのキーの入力状態を取得する
	result = devkeyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE KeyNumber)
{
	//指定キーを押していればtrueを返す
	if (key[KeyNumber])
	{
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE KeyNumber)
{
	//指定キーを押していればtrueを返す
	if (key[KeyNumber])
	{
		if (Oldkey[KeyNumber] == false)
		{
			return true;
		}
	}
	return false;
}
