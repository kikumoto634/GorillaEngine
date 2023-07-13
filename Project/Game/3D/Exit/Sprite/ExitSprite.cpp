#include "ExitSprite.h"
#include "Input.h"
#include "TextureUtility.h"

using namespace std;

void ExitSprite::Initialize()
{
	coinSp_ = make_unique<BaseSprites>();
	coinSp_->Initialize(coin_tex.number);
	coinSp_->SetSize({40,40});
	coinSp_->SetAnchorPoint({0.5f,0.5f});

	for(int i = 0; i < CoinSpNumSize; i++){
		coinSpNum_[i] = make_unique<BaseSprites>();
		coinSpNum_[i]->Initialize(cross_tex.number);
		coinSpNum_[i]->SetSize({40,40});
		coinSpNum_[i]->SetAnchorPoint({0.5f,0.5f});
	}
	NeedCoinSpriteUpdate();

	exitOpenSp_ = make_unique<BaseSprites>();
	buttonSpNumber = buttonZ_tex.number;
	exitOpenSp_->Initialize(buttonSpNumber);
	exitOpenSp_->SetSize({30,30});
	exitOpenSp_->SetAnchorPoint({0.5f,0.5f});
}

void ExitSprite::Update(bool isOpen, bool isPlayerContact)
{
	isPlayerContact_ = isPlayerContact;
	isOpen_ = isOpen;

	coinSp_->Update();
	for(int i = 0; i < CoinSpNumSize; i++){
		Vector2 pos = coinSp_->GetPosition() + Vector2{static_cast<float>(40+(i*40)), 0};
		coinSpNum_[i]->SetPosition(pos);
		coinSpNum_[i]->Update();
	}
	{
		//コントローラ接続確認
		buttonSpNumber = Input::GetInstance()->GetIsPadConnect()==true ? PadA_tex.number : buttonZ_tex.number;
		exitOpenSp_->SetTexNumber(buttonSpNumber);

		Vector2 pos = coinSp_->GetPosition() + Vector2{-40,20};
		exitOpenSp_->SetPosition(pos);
		exitOpenSp_->Update();
	}
}

void ExitSprite::Draw()
{
	if(!isPlayerContact_) return;
	coinSp_->Draw();
	for(int i = 0; i < CoinSpNumSize; i++){
		coinSpNum_[i]->Draw();
	}

	if(!isOpen_) return ;
	exitOpenSp_->Draw();
}

void ExitSprite::Finalize()
{
	coinSp_->Finalize();
	for(int i = 0; i < CoinSpNumSize; i++){
		coinSpNum_[i]->Finalize();
	}
	exitOpenSp_->Finalize();
}


void ExitSprite::NeedCoinSpriteUpdate()
{
	for(int i = 0; i < CoinSpNumSize; i++){		
		int value_Ten = exitNeedCoinsNum_/10;
		int value_One = exitNeedCoinsNum_ - value_Ten*10;

		if(i == 0)coinSpNum_[i]->SetTexNumber(16);
		else if(i == 1)coinSpNum_[i]->SetTexNumber(TexNumberBegin + value_Ten);
		else if(i == 2)coinSpNum_[i]->SetTexNumber(TexNumberBegin + value_One);
	}
}
