#pragma once
#include <windows.h>
#include <wrl.h>
#include "WinApp.h"

#define SIRECTINPUT_VERSION 0x0000	//DirectIputのバージョン指定
#include <dinput.h>

//入力
class Input
{
public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	//メンバ関数

	static Input* GetInstance();
	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();
	//キーの押下をチェック
	bool PushKey(BYTE KeyNumber);
	//キーのトリガーをチェック
	bool TriggerKey(BYTE KeyNumber);

private:	//メンバ変数
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> devkeyboard;
	//DirectInputのインスタンス
	ComPtr<IDirectInput8> dinput;
	//全キーの状態
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE Oldkey[256] = {};
	//WindowsAPI
	WinApp* winApp = nullptr;
};