#include "Upgrade_system.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include "Engine/Resources.hpp"


UpgradeSystem::UpgradeSystem() {}

void UpgradeSystem::Activate(Turret* turret) {
    active = true;
    targetTurret = turret;
    //PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    //scene->Pause();
}

void UpgradeSystem::Deactivate() {
    active = false;
    targetTurret = nullptr;
    //PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    //scene->Resume();
}

void UpgradeSystem::Update(float deltaTime) {
    // 目前這裡不用做更新邏輯
}

void UpgradeSystem::Draw() const {
    if (!active || !targetTurret) return;

    // 畫面半透明遮罩
    al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(0, 0, 0, 150));

    // 升級選項按鈕 (1~5級)
    for (int i = 0; i < 5; ++i) {
        float bx = 100 + i * 120;
        float by = 300;
        float bw = 100, bh = 100;

        ALLEGRO_COLOR color = (targetTurret->GetLevel() == i + 1) ? al_map_rgb(255, 215, 0) : al_map_rgb(255, 255, 255);
        al_draw_filled_rectangle(bx, by, bx + bw, by + bh, color);
        al_draw_rectangle(bx, by, bx + bw, by + bh, al_map_rgb(0, 0, 0), 3);
        al_draw_textf(Engine::Resources::GetInstance().GetFont("pirulen.ttf", 24).get(),
                       al_map_rgb(0, 0, 0), bx + bw / 2, by + bh / 2 - 12, ALLEGRO_ALIGN_CENTER, "Lv %d", i + 1);
    }
}

void UpgradeSystem::OnMouseDown(int button, int mx, int my) {
    if (!active || !targetTurret) return;

    if (button == 1) { // 左鍵
        for (int i = 0; i < 5; ++i) {
            float bx = 100 + i * 120;
            float by = 300;
            float bw = 100, bh = 100;
            if (mx >= bx && mx <= bx + bw && my >= by && my <= by + bh) {
                targetTurret->Upgrade(i + 1);
                Deactivate();
                break;
            }
        }
    }
}