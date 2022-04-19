#pragma once

#include "Object3d.h"
#include <memory>

//オブジェクトマネージャー
class ObjectManager
{
public:

	void Update();

	void Draw();

	/// <summary>
	/// オブジェクト追加
	/// </summary>
	/// <param name="object"></param>
	std::weak_ptr<Object3d> AddObject(std::shared_ptr<Object3d> object);

private:
	//オブジェクトリスト
	std::vector<std::shared_ptr<Object3d>> objects_;
};

