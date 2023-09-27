#include "LevelLoader.h"

#include <json.hpp>
#include <fstream>
#include <assert.h>

#define PI 3.14159265f


const std::string LevelLoader::kDefaultBaseDirectory = "Resources/Levels/";
const std::string LevelLoader::kExtension = ".json";

LevelData *LevelLoader::Load(const std::string &filename)
{
	const std::string fullpath = kDefaultBaseDirectory + filename + kExtension;

	std::ifstream file{};

	file.open(fullpath);
	if(file.fail()){
		assert(0);
	}

	//JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンス
	LevelData* levelData = new LevelData();

	//"objects"の全オブジェクトを走査
	for(nlohmann::json& object : deserialized["objects"]){
		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		//種類ごとの処理
		if(type.compare("MESH") == 0){
			//要素追加
			levelData->objects.emplace_back(LevelData::ObjectData{});
			//今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects.back();

			if(object.contains("file_name")){
				//ファイル名
				objectData.fileName = object["file_name"];

				//トランスフォームのパラメータ読み込み
				nlohmann::json& transform = object["transform"];
				//平行移動
				objectData.translation.x = -(float)transform["translation"][0];
				objectData.translation.y = (float)transform["translation"][2];
				objectData.translation.z = -(float)transform["translation"][1];
				//回転角
				objectData.rotation.x = (float)transform["rotation"][0]*(180.f/PI) - 90*(PI/180.f);
				objectData.rotation.y = (float)transform["rotation"][2]*(180.f/PI);
				objectData.rotation.z = (float)transform["rotation"][1]*(180.f/PI);
				//スケーリング
				objectData.scaling.x = (float)transform["scaling"][0];
				objectData.scaling.y = (float)transform["scaling"][1];
				objectData.scaling.z = (float)transform["scaling"][2];

				//コライダーのパラメータ読み込み
			}
		}


		//再帰関数
		if(object.contains("children")){
			
		}
	}

	return levelData;
}
