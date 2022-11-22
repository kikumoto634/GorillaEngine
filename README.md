# GorillaEngine

自作エンジン GorollaEngine(仮)


# 実装内容

スプライト実装
・application.cpp内のTextureManager.cppに描画したいスプライトpathとTexNumberを決める。
(pathの誤り、TexNumberの被りは避けてください。)

・シーンにてBaseSprite.h/.cppを継承したクラスを変数する。
(unique_ptrを使用すること。参考としてSampleSprite.h/.cppがある。)

・SceneにInitialize(),Update(),Draw(),Finalze()の中に#pragma にて2D処理の位置が指定されている。

Initialize() : sp = make_unique>SampleSprite>();, sp->Initialize(引数:(UINT texNumber));

Update()     : sp->Update(引数:(camera));

Draw()       :   sp->Draw();

Finalize()   : sp->Finalize();