#include "Player.h"

Player::Player(void)
{
    body_->coordinate_.SetPosition({ 0,20,0 });
}

void Player::Update(void)
{
    body_->Update();
}

void Player::Draw(void)
{
    body_->Draw("Resources/red1x1.png");
}
