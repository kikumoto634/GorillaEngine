#pragma once
#include <string>

struct AudioState{
	int number;
	float volume;
	std::string path;
};


//ワールド定義
inline AudioState rhythm_audio		= {0,  0.25f, "rhythm.wav"};
inline AudioState miss_audio		= {1,  0.7f,  "miss.wav"};
inline AudioState damage_audio		= {2,  0.3f,  "damage.wav"};
inline AudioState exBPM90_audio		= {3,  0.25f, "ex)_BPM90.wav"};
inline AudioState exBPM120_audio	= {4,  0.25f, "ex)_BPM120.wav"};
inline AudioState exBPM180_audio	= {5,  0.25f, "ex)_BPM180.wav"};
inline AudioState openExit_audio	= {6,  0.5f,  "OpenExit.wav"};
inline AudioState coinGet_audio		= {7,  0.5f,  "coinGet.wav"};
inline AudioState bpm120Game_audio	= {8,  0.5f,  "BPM120_Game.wav"};
inline AudioState bpm120Home_audio	= {9,  0.25f, "BPM120_Home.wav"};
inline AudioState dig_audio			= {10, 1.0f,  "dig.wav"};
inline AudioState gateEnter_audio	= {11, 0.25f, "Enter.wav"};
inline AudioState cutIn_audio		= {12, 0.25f, "cutin.wav"};
inline AudioState reflected_audio	= {13, 0.5f,  "reflected.wav"};
inline AudioState roar_audio	    = {14, 0.75f, "Roar.wav"};
inline AudioState recover_audio	    = {15, 0.75f, "Recover.wav"};
inline AudioState push_audio		= {18, 1.0f,  "push.wav"};