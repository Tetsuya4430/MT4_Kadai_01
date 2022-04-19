#include "MyGame.h"
#include "SceneFactory.h"

void MyGame::Initialize()
{
	//���N���X�̏���������
	FrameWork::Initialize();

	//�V�[���̏���������
	//�V�[���t�@�N�g���𐶐����A�}�l�[�W���[�ɃZ�b�g
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

	//�V�[���}�l�[�W���ɍŏ��̃V�[�����Z�b�g
	SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
}

void MyGame::Finalize()
{

	//���N���X�̏I������
	FrameWork::Finalize();
}

void MyGame::Update()
{
	//���N���X�̍X�V����
	FrameWork::Update();
}

void MyGame::Draw()
{
	FrameWork::Draw();
}