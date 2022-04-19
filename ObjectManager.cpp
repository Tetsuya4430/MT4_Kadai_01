#include "ObjectManager.h"

void ObjectManager::Update()
{
	for (std::shared_ptr<Object3d>& object : objects_)
	{
		object->Update();
	}
}

void ObjectManager::Draw()
{
	for (std::shared_ptr<Object3d>& object : objects_)
	{
		object->Draw();
	}
}

std::weak_ptr<Object3d> ObjectManager::AddObject(std::shared_ptr<Object3d> object)
{
	//オブジェクトリストに登録
	objects_.push_back(std::move(object));

	return objects_.back();
}
