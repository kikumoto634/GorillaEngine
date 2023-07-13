#include "Combo.h"

#include "Easing.h"

using namespace std;

void Combo::Initialize()
{
	//コンボテキスト
	comboSp_ = make_unique<BaseSprites>();
	comboSp_->Initialize(combo_tex.number);
	comboSp_->SetPosition(PositionText);
	comboSp_->SetSize(SizeText);
	comboSp_->SetColor({comboSpColor_.x,comboSpColor_.y,comboSpColor_.z,1});

	//数字
	for(int i = 0;i < NumberSpSize; i++){
		numberSp_combo_[i] = make_unique<BaseSprites>();
		numberSp_combo_[i]->Initialize(TexNumberBegin + 0);
		numberSp_combo_[i]->SetPosition({float(PositionNumber.x+(i*SizeNumber.x)),PositionNumber.y});
		numberSp_combo_[i]->SetSize(SizeNumber);
	}
}

void Combo::Update()
{
	comboSp_->SetColor({comboSpColor_.x,comboSpColor_.y,comboSpColor_.z,1});
	comboSp_->Update();
	//数字
	for(int i = 0;i < NumberSpSize; i++){
		numberSp_combo_[i]->SetColor({comboSpColor_.x,comboSpColor_.y,comboSpColor_.z,1});
		numberSp_combo_[i]->Update();
	}

	//リセット色変化
	if(isReset_){
		comboSpColor_.x = Easing_Point2_Linear<float>(0.0f,1.0f,Time_OneWay(curColorChangeFrame_,ComboResetColorSecond));
		comboSpColor_.y = comboSpColor_.x;

		if(comboSpColor_.x >= 1){
			isReset_ = false;
			curColorChangeFrame_ = 0;
		}
	}
}

void Combo::Draw()
{
	comboSp_->Draw();
	//数字
	for(int i = 0;i < NumberSpSize; i++){
		numberSp_combo_[i]->Draw();
	}
}

void Combo::Finalize()
{
	//数字
	for(int i = 0;i < NumberSpSize; i++){
		numberSp_combo_[i]->Finalize();
	}
	comboSp_->Finalize();
}

void Combo::Increment()
{
	comboNum_ += IncrementValue;

	//スプライト更新
	int hundred = comboNum_/ValueHundred;
	int ten = (comboNum_ - (hundred*ValueHundred))/ValueTen;
	int one = (comboNum_ - (hundred*ValueHundred) - (ten*ValueTen))/ValueOne;
	numberSp_combo_[0]->SetTexNumber(hundred + TexNumberBegin);
	numberSp_combo_[1]->SetTexNumber(ten + TexNumberBegin);
	numberSp_combo_[2]->SetTexNumber(one + TexNumberBegin);
}

void Combo::Reset()
{
	comboNum_ = 0;
	//スプライト更新
	for(int i = 0; i <NumberSpSize; i++){
		numberSp_combo_[i]->SetTexNumber(TexNumberBegin);
	}

	//色変更
	if(isReset_) return;
	isReset_ = true;
	comboSpColor_ = ResetColor;
}
