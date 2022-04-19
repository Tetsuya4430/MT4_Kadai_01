#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public BaseScene
{
public:
	//初期化
	void Initialize() override;

	//終了処理
	void Finalize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;


private:
	Sprite* sprite = nullptr;
};

