#ifndef STAGESELECTSCENE_HPP
#define STAGESELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <unordered_map>

#include "Engine/IScene.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

class StageSelectScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    Engine::ImageButton* play_button;
    Engine::Sprite* stage_preview;
    std::vector<Engine::Sprite*> previews;
    Engine::Label* info;
    std::unordered_map<int, int> difficulty;
    int page;
    int maxPage;
    explicit StageSelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Draw() const override;
    void PlayOnClick(int stage);
    void ScoreboardOnClick();
    void BackOnClick(void);
    void PreviewStage(int stage);
};

#endif   // STAGESELECTSCENE_HPP
