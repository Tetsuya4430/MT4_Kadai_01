#pragma once

#include "BaseScene.h"

#include <string>

/// <summary>
/// �V�[���H��(�T�O)
/// </summary>
class AbstractSceneFactory
{
public:

	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// �V�[������
	/// </summary>
	/// /// <param name="sceneName">�V�[����</param>
	/// <returns></returns>
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;

};
