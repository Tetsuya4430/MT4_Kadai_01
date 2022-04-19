#include "Sprite.h"

#include <d3dx12.h>

using namespace DirectX;

Sprite* Sprite::Create( UINT texNumber, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	//メモリ確保
	Sprite* instance = new Sprite();
	//インスタンス初期化
	instance->Initialize( texNumber, anchorpoint, isFlipX, isFlipY);

	return instance;
}

void Sprite::Initialize(UINT texNumber, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	HRESULT result = S_FALSE;


	//メンバ変数に書き込む
	texNumber_ = texNumber;
	anchorpoint_ = anchorpoint;
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;

	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	//頂点データ
	VertexPosUv vertices[] = {
		{{0.0f, 100.0f, 0.0f}, {0.0f, 1.0f}},//左下
		{{0.0f,   0.0f, 0.0f}, {0.0f, 0.0f}},//左上
		{{100.0f, 100.0f, 0.0f}, {1.0f, 1.0f}},//右下
		{{100.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},//右上
	};

	//指定番号の画像が読み込み済みなら
	//if (spriteCommon->texBuff_[texNumber_])
	if(spriteCommon->GetTexBuff(texNumber))
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = spriteCommon->GetTexBuff(texNumber)->GetDesc();

		//スプライトの大きさを画像の解像度に合わせる
		size_ = { (float)resDesc.Width, (float)resDesc.Height };
		texSize_ = { (float)resDesc.Width, (float)resDesc.Height };
	}

	//頂点バッファ生成
	result = spriteCommon->GetDevice_()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_)
	);

	//---頂点バッファへのデータ転送---//
	//VertexPosUv* vertMap = nullptr;
	//result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	//memcpy(vertMap, vertices, sizeof(vertices));
	//vertBuff_->Unmap(0, nullptr);

	TransferVertexBuffer();

	//頂点バッファビューの生成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(vertices);
	vbView_.StrideInBytes = sizeof(vertices[0]);

	//定数バッファの生成
	result = spriteCommon->GetDevice_()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_)
	);

	//定数バッファにデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);		//色指定(RGBA)
	constMap->mat = spriteCommon->GetmatProjection();
	constBuff_->Unmap(0, nullptr);
}

void Sprite::TransferVertexBuffer()
{
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	HRESULT result = S_FALSE;

	//頂点データ
	VertexPosUv vertices[] = {
		{{}, {0.0f, 1.0f}},	//左下
		{{}, {0.0f, 0.0f}},	//左下
		{{}, {1.0f, 1.0f}},	//右下
		{{}, {1.0f, 0.0f}},	//右上
	};

	//左下, 左上,右下,右上
	enum { LB, LT, RB, RT };



	float left = (0.0f - anchorpoint_.x) * size_.x;
	float right = (1.0f - anchorpoint_.x) * size_.x;
	float top = (0.0f - anchorpoint_.y) * size_.y;
	float bottom = (1.0f -anchorpoint_.y) *size_.y;

	if (isFlipX_)
	{
		//左右反転
		left = -left;
		right = -right;
	}

	if (isFlipY_)
	{
		//上下反転
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,	bottom, 0.0f };
	vertices[LT].pos = { left,	top,	0.0f };
	vertices[RB].pos = { right,	bottom, 0.0f };
	vertices[RT].pos = { right,	top,	0.0f };

	if (spriteCommon->GetTexBuff(texNumber_))
	{
		D3D12_RESOURCE_DESC resDesc = spriteCommon->GetTexBuff(texNumber_)->GetDesc();

		float tex_left = texLeftTop_.x / resDesc.Width;
		float tex_right = (texLeftTop_.x + texSize_.x) / resDesc.Width;
		float tex_top = texLeftTop_.y / resDesc.Height;
		float tex_bottom = (texLeftTop_.y + texSize_.y) / resDesc.Height;

		vertices[LB].uv = { tex_left,	tex_bottom };
		vertices[LT].uv = { tex_left,	tex_top };
		vertices[RB].uv = { tex_right,	tex_bottom };
		vertices[RT].uv = { tex_right,	tex_top };

	}


	//頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff_->Unmap(0, nullptr);
}

void Sprite::Draw()
{
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	//非表示フラグがtrueなら
	if (isInvisible_)
	{
		//描画せずに抜ける
		return;
	}

	ID3D12GraphicsCommandList* commandList = spriteCommon->GetCommandList();

	//頂点バッファのセット
	commandList->IASetVertexBuffers(0, 1, &vbView_);

	//ルートパラメータ0番に定数バッファをセット
	commandList->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	//ルートパラメータ1番にシェーダーリソースビューをセット
	spriteCommon->SetGraphicsRootDescriptorTable(1, texNumber_);

	//ポリゴンの描画
	commandList->DrawInstanced(4, 1, 0, 0);
}

void Sprite::Update()
{
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();

	//ワールド行列の更新
	matWorld_ = XMMatrixIdentity();

	//z軸回転
	matWorld_ *= XMMatrixRotationZ(XMConvertToRadians(rotation_));

	//平行移動
	matWorld_ *= XMMatrixTranslation(position_.x, position_.y,position_.z);

	//定数バッファの転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matWorld_ * spriteCommon->GetmatProjection();
	constMap->color = color_;
	constBuff_->Unmap(0, nullptr);

}
