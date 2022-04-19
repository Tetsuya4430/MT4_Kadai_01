#pragma once
#include "Sprite.h"

#include <windows.h>
#include <string>

//デバッグ文字列クラスの定義
class DebugText
{
public:
	//ここに定数宣言を記述
	static const int maxCharCount = 256;	//1フレームでの最大表示文字数
	static const int fontWidth = 25;		//フォント画像内1文字分の横幅
	static const int fontHeight = 25;		//フォント画像内1文字分の縦幅
	static const int fontLineCount = 10;	//フォント画像内1桁分の文字数

//ここにメンバ関数の宣言を記述
	static DebugText* GetInstance();

	void Initialize(SpriteCommon* spriteCommon, UINT texnumber);
	void Print(const std::string& text, float x, float y, float scale = 1.0f);
	void DrawAll();

	void Finalize();
private:
	//ここにメンバ変数の宣言を記述

		//スプライト共通部分(借りてくる)
	SpriteCommon* spriteCommon_ = nullptr;
	//スプライトの配列
	Sprite* sprites_[maxCharCount];
	//スプライト配列の添え字番号
	int spriteIndex_ = 0;
};