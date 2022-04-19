#pragma once

/// <summary>
/// 前方宣言
/// </summary>
class SceneManager;

/// <summary>
/// シーン基底
/// </summary>
class BaseScene
{
public:
	virtual ~BaseScene() = default;

	//初期化
	virtual void Initialize() = 0;

	//終了処理
	virtual void Finalize() = 0;

	//更新
	virtual void Update() = 0;

	//描画
	virtual void Draw() = 0;

};

