# GorillaEngine

自作エンジン GorollaEngine(仮)


# 実装内容

スプライト出力
・application.cpp内のTextureManager.cppに描画したいスプライトpathとTexNumberを決める。
(pathの誤り、TexNumberの被りは避けてください。)

・シーンにてBaseSprite.h/.cppを継承したクラスを変数する。
(unique_ptrを使用すること。参考としてSampleSprite.h/.cppがある。)

・SceneにInitialize(),Update(),Draw(),Finalze()の中に#pragma にて2D処理の位置が指定されている。
Initialize() : sp = make_unique>SampleSprite>();    
               sp->Initialize(引数:(UINT texNumber));
Update()     : sp->Update(引数:(camera));
Draw()       : sp->Draw();
Finalize()   : sp->Finalize();


幾何学オブジェクト出力
・スプライトと同様、application.cpp内のTextureManager.cppにオブジェクトに貼り付けたいスプライトpathとTexNumberを決める。
(pathの誤り、TexNumberの被りは避けてください。)

・シーンにてBaseSprite.h/.cppを継承したクラスを変数する。
(unique_ptrを使用すること。参考としてSampleGeometryObject.h/.cppがある。)

・SceneにInitialize(),Update(),Draw(),Finalze()の中に#pragma にて2D処理の位置が指定されている。
Initialize() : sp = make_unique>SampleSprite>();    
               sp->Initialize(引数:(UINT texNumber));
Update()     : sp->Update(引数:(camera));
Draw()       : sp->Draw();
Finalize()   : sp->Finalize();


Fbxオブジェクト出力
・シーンにてBaseSprite.h/.cppを継承したクラスを変数する。
(unique_ptrを使用すること。参考としてSampleFbxObject.h/.cppがある。)

・SceneにInitialize(),Update(),Draw(),Finalze()の中に#pragma にて2D処理の位置が指定されている。
Initialize() : sp = make_unique>SampleSprite>();    
               sp->Initialize(引数:(std::string filePath)));
Update()     : sp->Update(引数:(camera));
Draw()       : sp->Draw();
Finalize()   : sp->Finalize();


上記三つ共通の関数
・Getter/Setter
Position();
Rotation();


Easing関数
・ゲームオブジェクトにて指定した位置まで移動してほしいときに使用。

#include "../Engine/math//Easing/Easing.h"

Vctor3 pos = Easing_Linear_Point〇(Vector3 StartPos(), … Vector3 endPos(), float frame);    (〇には指定できる座標数が入る)
指定したフレームが1になるまでに,EndPos()まで移動。

・Easingに使用するframeの加算関数

Time_OneWay(float& frame, const float& second);
Time_Loop(float& frame, const float& second);
指定したsecond分立つまでのframeを返す。
