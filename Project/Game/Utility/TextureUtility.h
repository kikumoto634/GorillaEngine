#pragma once
#include <string>

struct TextureState{
	int number;
	std::string path;
};

inline TextureState texFont_tex			 =	{0,  "texfont.png"};
inline TextureState white1x1_tex		 =	{1,  "white1x1.png"};
inline TextureState uvChecker_tex		 =	{2,  "uvChecker.png"};