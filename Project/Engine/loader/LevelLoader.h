#pragma once
#include <string>
#include <vector>
#include "Vector3.h"

// レベルデータ
struct LevelData {

	struct ObjectData {
		// ファイル名
		std::string fileName;
		// 平行移動
		Vector3 translation;
		// 回転角
		Vector3 rotation;
		// スケーリング
		Vector3 scaling;
	};

	// オブジェクト配列
	std::vector<ObjectData> objects;
};

class LevelLoader
{
private:
	static const std::string kDefaultBaseDirectory;
	static const std::string kExtension;

public:
	static LevelData* Load(const std::string& filename);
};

