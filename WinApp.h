#pragma once
#include <windows.h>

#define WINDOW_H_
#define WINDOW_CLASS_NAME "DirectXGame"	//!< ウィンドウクラス名 Window class name


//WindowAPI
class WinApp
{

public:
	static const wchar_t windowClassName[];

public:	//静的メンバ関数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;

	//ウィンドウクラスの設定
	WNDCLASSEX w{};

public:	//メンバ関数
	//WindowsAPI関連の初期化
	void Initialize();
	//getter
	HWND GetHwnd() { return hwnd; }
	HINSTANCE GetHInstance() { return w.hInstance; }
	//メッセージの処理
	bool ProcessMessage();
	//終了
	void Finalie();
	

public:	//定数
	//ウィンドウ横幅
	static const int window_width = 1280;
	//ウィンドウ縦幅
	static const int window_height = 720;
};