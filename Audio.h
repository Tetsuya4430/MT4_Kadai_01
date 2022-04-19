#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>	//ComPtrを使うのに必要

class XAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:
	// ボイス処理パスの開始時
	//STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
	void OnVoiceProcessingPassStart(UINT32 BytesRequired) {};
	// ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
	// バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd) (THIS) {};
	// バッファの使用開始時
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
	// バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {
		// バッファを解放する
		delete[] pBufferContext;
	};
	// 再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
	// ボイスの実行エラー時
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
};

class Audio
{
private:
	//Microsoft::WRLを省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	//サブクラス
	//チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];		//チャンクごとのID
		int size;	//チャンクサイズ
	};

	//RIFTヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];		//"WAVE"
	};

	//MFTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt"
		WAVEFORMAT fmt;	//波形フォーマット
	};

public:	//メンバ関数

	static Audio* GetInstance();

	//初期化
	bool Initialize();

	//サウンドファイルの読み込みと再生(.wavのみ)
	void PlayMusic(const char* filename);

	//解放
	void Finalize();

private:	//メンバ変数
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2VoiceCallback voiceCallback;
};