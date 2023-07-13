#pragma once
#include "BaseObjObject.h"

class TouchableObject : public BaseObjObject
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	void Initialize(std::string filePath, bool IsSmmothing = false);
};

