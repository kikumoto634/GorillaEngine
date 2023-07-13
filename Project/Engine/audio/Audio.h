#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <unordered_map>
#include <string>

/// <summary>
/// サウンド再生管理
/// </summary>
class Audio
{
public://1エイリアス
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template<class T1, class T2> using unordered_map = std::unordered_map<T1,T2>;

public://インナークラス
	//チャンクヘッダー
	struct ChunkHeader
	{
		char id[4];//チャンク毎のID
		int size;	//チャンクサイズ
	};
	//RIFFヘッダチェック
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];	//"WAVE"
	};
	//FMTチェック
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt"
		WAVEFORMATEX fmt;	//波形フォーマット
	};

	//音声データ
	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex{};
		//波形データ
		char* pBuffer;
		//波形データのサイズ
		unsigned int dataSize;
	};

public://メンバ関数

	static void Load(UINT number, std::string filename);

	static Audio* GetInstance();

	~Audio();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// サウンドファイルの読み込み
	/// </summary>
	/// <param name="number">サウンド番号</param>
	/// <param name="filename">wavファイル名</param>
	void LoadWave(int number, const char* filename);

	//サウンド再生
	void PlayWave(int number, float volume = 1.0f, bool IsLoop = false);

	void StopWave(int number);

	void SetRation(int number, float ratio);

private://メンバ変数
	//XAudio2のインスタンス
	IXAudio2* xAudio2 = nullptr;
	//マスターボイス
	IXAudio2MasteringVoice* masterVoice = nullptr;
	//波形データの連想配列
	unordered_map<int, SoundData> soundDatas;

	unordered_map<int, IXAudio2SourceVoice*> LoopSound;
};

