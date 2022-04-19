#include "DirectXCommon.h"
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

void DirectXCommon::Initialize(WinApp* winApp)
{
	//NULL検出	
	assert(winApp);

	//メンバ変数に記録
	this->winApp = winApp;

	HRESULT result;

#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ID3D12Debug* debugController;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif


	//DXGIファクトリ (デバイス生成後は解放されてよい)
	ComPtr<IDXGIFactory6> dxgiFactory;

	//DXGIファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));


	//アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter1>> adapters;

	//ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter1> tmpAdapter;

	for (int i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);	//動的配列に追加する
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);	//アダプターの情報を取得

		//ソフトウェアデバイスを回避
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		std::wstring strDesc = adesc.Description;	//アダプター名

		//Intel UHD Graphics (オンボードグラフィック)を回避
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i].Get();	//採用
			break;
		}
	}

	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));

		if (result == S_OK)
		{
			//デバイスを生成出来た時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion

	//---コマンドリスト---//(GPUにまとめて命令を送る)
#pragma region コマンドリスト
//コマンドアロケーターを生成
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator)
	);

	//コマンドリストを生成
	result = dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList)
	);

	//---コマンドキュー---//(コマンドリストをGPUに順に実行させていく為の仕組み)
	//標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};	//構造体の中身を0でクリアしている

	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

#pragma endregion

	//---スワップチェーン---//(ダブルバッファリングをするために必要な仕組み)
#pragma region スワップチェーン
//各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//色情報の書式
	swapchainDesc.SampleDesc.Count = 1;	//マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	//バックバッファ用
	swapchainDesc.BufferCount = 2;	//バッファ数を2つに指定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1のComPtrを用意
	ComPtr<IDXGISwapChain1> swapchain1;

	dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		winApp->GetHwnd(),
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1
	);

	//生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	swapchain1.As(&swapchain);

#pragma endregion

	//---レンダーターゲットビュー---//(バックバッファを描画キャンバスとして扱うためのオブジェクト)
#pragma region レンダーターゲットビュー
//各種設定をしてデスクリプタヒープを生成
	//D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//レンダーターゲットビュー
	heapDesc.NumDescriptors = 2;	//表裏の2つ


	//表裏の2つ分のポインタを用意
	backBuffers.resize(2);

	for (int i = 0; i < (int)heapDesc.NumDescriptors; i++)
	{
		dev->CreateDescriptorHeap(
			&heapDesc,
			IID_PPV_ARGS(&rtvHeaps)
		);
	}



	for (int i = 0; i < 2; i++)
	{
		//スワップチェーンからバッファを取得
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),	//ヒープ先頭のアドレス
				i,	//ディスクリプタ番号
				dev->GetDescriptorHandleIncrementSize(heapDesc.Type)	//ディスクリプタ1つ分のサイズ
			);

		//レンダーターゲットビューの生成
		dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),	//ヒープ先頭のアドレス
				i,	//ディスクリプタ番号
				dev->GetDescriptorHandleIncrementSize(heapDesc.Type)	//ディスクリプタ1つ分のサイズ
			)
		);
	}
#pragma endregion

	//---深度テスト---//(既に描画キャンバスに書かれているものより手前の時だけ描くようにすることで前後関係が守られる)
	//※描画初期化処理に描くのが好ましいが「レンダーターゲット」「スワップチェーン」に関係するので近くに書いている
#pragma region 深度テスト関係

//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;	//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//デプスステンシルビュー
	//ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//ComPtr<ID3D12Resource> depthBuffer;

	//深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WinApp::window_width,
		WinApp::window_height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	//深度バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

#pragma endregion


	//---フェンス---//(CPUとGPUで同期を取るための仕組み)
#pragma region フェンス
//フェンスの生成
	//ComPtr<ID3D12Fence> fence;
	/*UINT64 fenceVal = 0;*/

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

}

void DirectXCommon::PreDraw()
{
	//バックバッファの番号を取得
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();


	//1,リソースバリアで書き込み可能に変更
		//表示状態から描画状態に変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

#pragma endregion

	//2,描画先指定(バックバッファを描画先として指定)
#pragma region 描画先指定コマンド
//レンダーターゲットビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);

	//深度ステンシルビュー用のデスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
#pragma endregion

	//3,画面クリア
	float clearColor[] = { 0.0f, 0.25f, 0.25f, 0.0f };	//画面の初期カラー
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


//	//---ビューポート設定---//
//	//ビューポートの設定コマンド
//	D3D12_VIEWPORT viewport{};
//
//	viewport.Width = WinApp::window_width;
//	viewport.Height = WinApp::window_height;
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::window_width, WinApp::window_height));
//
//	//---シザー矩形設定---//
//	D3D12_RECT scissorrect{};
//
//	scissorrect.left = 0;
//	scissorrect.right = scissorrect.left + WinApp::window_width;
//	scissorrect.top = 0;
//	scissorrect.bottom = scissorrect.top + WinApp::window_height;
//
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, WinApp::window_width, WinApp::window_height));

}

void DirectXCommon::PostDraw()
{
	

	//バックバッファの番号を取得
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//5,リソースバリアを戻す(バックバッファが書き込み可能状態のままだと画面表示に使えないので{D3D12_RESOURCE_STATE_PRESENT}画面表示状態に変更する)
#pragma region リソースバリア書き込み禁止
//表示状態から描画状態に変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

#pragma endregion

	//---コマンドのフラッシュ---//(ここまで貯めたコマンドを実行し描画する)
#pragma region コマンドの実行
//命令のクローズ
	cmdList->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	//コマンドリストの配列
	cmdQueue->ExecuteCommandLists(1, cmdLists);

	//バッファをフリップ(裏表の入れ替え)
	swapchain->Present(1, 0);

	//コマンドキューの実行完了を待つ
	cmdQueue->Signal(fence.Get(), ++fenceVal);

	if (fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
#pragma endregion
	//画面入れ替え(次の仕様に備え、コマンドアロケータとコマンドリストをリセットする)
#pragma region 画面入れ替え
	cmdAllocator->Reset();	//キューをクリア
	cmdList->Reset(cmdAllocator.Get(), nullptr);	//再びコマンドリストを貯める準備
#pragma endregion
}
