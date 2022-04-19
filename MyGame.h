#include "FrameWork.h"

#include <vector>

/// <summary>
/// ゲーム固有のクラス
/// </summary>
class MyGame : public FrameWork
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:
	
};
