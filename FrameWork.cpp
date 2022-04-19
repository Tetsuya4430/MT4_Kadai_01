#include "FrameWork.h"

void FrameWork::Run()
{
	//ゲームの初期化
	Initialize();

	while (true)	//ゲームループ
	{
		//毎フレーム更新
		Update();

		//終了リクエストが来たら抜ける
		if (IsEndRequest())
		{
			break;
		}
		//描画
		Draw();
	}
	//ゲームの終了
	Finalize();
}

void FrameWork::Initialize()
{
	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//スプライト共通部分の初期化
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon->GetDev(), dxCommon->GetCmdList(), winApp->window_width, winApp->window_height);

	//デバッグテキスト
	debugText = DebugText::GetInstance();

	//デバッグテキスト用のテクスチャ番号を指定
	const int debugTextTexNumber = 0;
	//デバッグテキスト用のテクスチャ読み込み
	spriteCommon->SpriteCommonLoadTexture(debugTextTexNumber, L"Resources/Font/ASCII_Font.png");
	//デバッグテキスト初期化
	debugText->Initialize(spriteCommon, debugTextTexNumber);

	//入力の初期化
	input = Input::GetInstance();
	input->Initialize(winApp);

	//オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	//3Dオブジェクトの静的初期化
	Object3d::StaticInitialize(dxCommon->GetDev(), dxCommon->GetCmdList(), WinApp::window_width, WinApp::window_height);

}

void FrameWork::Finalize()
{
	//シーンファクトリ解放
	delete sceneFactory_;

	//デバッグテキスト解放
	//debugText->Finalize();

	//オーディオ解放
	audio->Finalize();


	//DirectX解放
	delete dxCommon;

	//WindowsのAPIの終了処理
	winApp->Finalie();

	//WindowsAPIの解放
	delete winApp;
}

void FrameWork::Update()
{
	//Windowsのメッセージ処理
	if (winApp->ProcessMessage())
	{
		//ゲームループを抜ける
		endRequest_ = true;
		return;
	}

	//入力の更新
	input->Update();

	//シーンの更新
	SceneManager::GetInstance()->Update();
}

void FrameWork::Draw()
{
	//描画前処理
	dxCommon->PreDraw();

	SceneManager::GetInstance()->Draw();

	//デバッグテキスト描画
	debugText->DrawAll();


	//4,描画コマンドここまで

	dxCommon->PostDraw();
}
