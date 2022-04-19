#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "WinApp.h"

using namespace Microsoft::WRL;

//DirectX基盤
class DirectXCommon
{
public:	//メンバ関数
	//初期化
	void Initialize(WinApp* winApp);
	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();
	//デバイス取得getter
	ID3D12Device* GetDev() { return dev.Get(); }
	//コマンドリスト取得getter
	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }


private:	//メンバ変数
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12Fence> fence;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};


	UINT64 fenceVal = 0;

	//WindowsAPI
	WinApp* winApp = nullptr;
	

private:
	//デバイス
	ComPtr<ID3D12Device> dev;
	//DXGIファクトリ
	ComPtr<IDXGIFactory6> dxgiFactory;
};