#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Input.h"
#include "ObjectManager.h"

#include <memory>

/// <summary>
/// �Q�[���v���C�V�[��
/// </summary>
class GamePlayScene : public BaseScene
{
public:
	//������
	void Initialize() override;

	//�I������
	void Finalize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	
private:
	Sprite* sprite = nullptr;
	Model* model_1 = nullptr;
	Model* model_2 = nullptr;
	Input* input = nullptr;

	std::shared_ptr<Object3d> object3d_1;
	std::shared_ptr<Object3d> object3d_2;
	std::shared_ptr<Object3d> object3d_3;
	std::unique_ptr<ObjectManager> objectManager_1;
	std::unique_ptr<ObjectManager> objectManager_2;
	std::unique_ptr<ObjectManager> objectManager_3;

	float VY = 0.0f;	//Y�����̑��x
	const float gravity = -9.8f / 60.0f;	//�d��
	float y = 60.0f;	//�����̏����l


};

