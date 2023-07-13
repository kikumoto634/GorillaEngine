#include "Audio.h"
#include <assert.h>
//ファイル読み込み
#include <fstream>

#pragma comment(lib, "xaudio2.lib")
using namespace std;

void Audio::Load(UINT number, std::string filename)
{
	std::string fullPath = "Resources/sound/" + filename;
	Audio::GetInstance()->LoadWave(number, fullPath.c_str());
}

Audio *Audio::GetInstance()
{
	static Audio instance;
	return &instance;
}

Audio::~Audio()
{
	//読み込み済みサウンドの波形データを解放
	for(auto& pair : soundDatas)
	{
		delete pair.second.pBuffer;
	}
	//soundDatas = {};
}

void Audio::Initialize()
{
	HRESULT result;

	//XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
}

void Audio::Finalize()
{
	masterVoice->DestroyVoice();
	//delete masterVoice;
	//masterVoice = nullptr;

	xAudio2->Release();
	//delete xAudio2;
	//xAudio2 = nullptr;

	//読み込み済みサウンドの波形データを解放
	/*for(auto& pair : soundDatas)
	{
		delete pair.second.pBuffer;
	}*/
	//soundDatas = {};
}

void Audio::LoadWave(int number, const char *filename)
{
	SoundData soundData{};

	////ファイルオープン
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリーモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	////.wavデータ読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff{};
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if(strncmp(riff.chunk.id, "RIFF",4) != 0)
	{
		assert(0);
	}
	//タイプがWAVEかチェック
	if(strncmp(riff.type, "WAVE",4) != 0)
	{
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if(strncmp(format.chunk.id, "fmt ", 4) != 0)
	{
		assert(0);
	}
	//読み込んだ音声データをreturn
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//Dataチャンク読み込み
	ChunkHeader data{};
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if(strncmp(data.id, "JUNK", 4) == 0)
	{
		//読み込み1をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data,sizeof(data));
	}
	if(strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}
	//波形データのサイズ記録
	soundData.dataSize = data.size;

	//Dataチャンクのデータ部(波形データ)の読み込み
	soundData.pBuffer =new char[data.size];
	file.read(soundData.pBuffer, data.size);

	//Waveファイルを閉じる
	file.close();

	//波形フォーマットの設定
	memcpy(&soundData.wfex, &format.fmt, sizeof(format.fmt));
	soundData.wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;


	//連想配列に要素を追加
	soundDatas.insert(std::make_pair(number, soundData));
}

void Audio::PlayWave(int number, float volume, bool IsLoop)
{
	SoundData& soundData = soundDatas[number];

	HRESULT result = S_FALSE;

	//波形フォーマットをもとにSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice= nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex, 0, 2.0f, nullptr);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)soundData.pBuffer;
	buf.pContext = soundData.pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = soundData.dataSize;

	if(LoopSound.find(number)!=LoopSound.end()){
		return;
	}

	//ループ
	if(IsLoop){
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
		LoopSound.insert(make_pair(number, pSourceVoice));
	}

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->SetVolume(volume);
	result = pSourceVoice->Start();
}

void Audio::StopWave(int number)
{
	IXAudio2SourceVoice* pSourceVoice= nullptr;
	if(LoopSound.find(number)==LoopSound.end()) {
		return;
	}
	pSourceVoice = LoopSound.at(number);

	pSourceVoice->Stop();
	LoopSound.erase(number);
}

void Audio::SetRation(int number, float ratio)
{
	IXAudio2SourceVoice* pSourceVoice= nullptr;
	if(LoopSound.find(number)==LoopSound.end()) {
		return;
	}
	pSourceVoice = LoopSound.at(number);

	pSourceVoice->SetFrequencyRatio(ratio);
}

