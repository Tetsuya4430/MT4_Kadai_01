#pragma once

#include "PipelineSet.h"

#include <DirectXMath.h>

/// <summary>
/// スプライト共通部分
/// </summary>
class SpriteCommon
{
public:
	static SpriteCommon* GetInstance();

	//テクスチャの最大枚数
	static const int spriteSRVCount = 512;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="window_width">ウィンドウ横幅</param>
	/// <param name="window_height">ウィンドウ縦幅</param>
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int window_width, int window_height);


/// <summary>
/// 描画前処理
/// </summary>
	void PreDraw();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="texnumber"></param>
	/// <param name="filename"></param>
	void SpriteCommonLoadTexture(UINT texnumber, const wchar_t* filename);

	/// <summary>
	/// ルートデスクリプタテーブルの設定
	/// </summary>
	/// <param name="rootParameterIndex">ルートパラメータ番号</param>
	/// <param name="texnumber">テクスチャ番号</param>
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, UINT texnumber);


	/// <summary>
	/// テクスチャ取得
	/// </summary>
	/// <param name="texnumber">テクスチャ番号</param>
	/// <returns>テクスチャリソース</returns>
	ID3D12Resource* GetTexBuff(int texNumber);

	ID3D12Device* GetDevice_() { return device_; }
	const DirectX::XMMATRIX& GetmatProjection() { return matProjection_; }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_; }

private:
	//パイプラインセット
	PipelineSet pipelineSet_;

	//射影行列
	DirectX::XMMATRIX matProjection_{};

	//テクスチャ用デスクリプタヒープ生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap_;

	//テクスチャリソース(テクスチャバッファ)の配列
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_[spriteSRVCount];

	//デバイス(借りてくる)
	ID3D12Device* device_ = nullptr;

	//コマンドリスト(借りてくる)
	ID3D12GraphicsCommandList* commandList_ = nullptr;

private:
	//パイプライン生成
	void CreateGraphicsPipeline();
};

