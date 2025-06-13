#include <allegro5/color.h>
#include "CoinGen.hpp"
#include "Scene/PlayScene.hpp"

const int CoinGen::Price = 20; // 放置需要 100 金幣

CoinGen::CoinGen(float x, float y)
    : Turret("play/tower-base.png", "play/farm.png", x, y, 0, Price, 5.0f) { // cooldown 每 5 秒產一次
}

void CoinGen::Update(float deltaTime) {
    // 更新底座和動畫
    imgBase.Position = Position;
    imgBase.Tint = Tint;

    up_cost = 70;

    // 不需要攻擊邏輯
    reload -= deltaTime;
    if (reload <= 0+0.02*level) {
        reload = coolDown;

        // 產生金幣（加到玩家金幣數）
        getPlayScene()->EarnMoney(5); // 每次給玩家 10 金幣

        // 也可以做動畫、音效提示
        //AudioHelper::PlayAudio("coin.wav");

        PlayScene* scene = getPlayScene();
        scene->floatingTexts.push_back({
            Position,        // 文字出現的位置
            "+5",            // 文字內容
            1.0f              // 1 秒後消失
        });
    }
}

void CoinGen::CreateBullet()
{
    //left empty
}
