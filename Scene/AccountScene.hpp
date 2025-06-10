#ifndef ACCOUNTSCENE_HPP
#define ACCOUNTSCENE_HPP

#include "AccountSystem/GlobalAccountSystem.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/TextBox.hpp"
#include <allegro5/allegro_audio.h>

#include "Engine/IScene.hpp"
class AccountScene final : public Engine::IScene {
  private:
    Engine::TextBox *nameTextBox;
    Engine::TextBox *passwordTextBox;
    Engine::TextBox *activeTextBox;

    std::string userName;
    std::string userPass;

    GlobalAccountManager& accountManager = Engine::GameEngine::GetAccountManager();

    std::string statusMsg;
    float statusMsgTimer;
    Engine::ImageButton* popup;
  public:
    bool passHide;
    explicit AccountScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;

    void OnMouseDown(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnCharInput(int unicodeChar);

    void BackOnClick(void);
    void SubmitOnClick(void);
    void ToggleVisibility(void);
    void ClosePopUp(void);
    void LogoutOnClick(void);

    void ShowStatusMsg(const std::string& msg, float duration=5.0f);

    Engine::Image* bg;
    Engine::Image* cloud1;
    Engine::Image* cloud2;

    float x1, x2;
    float cloud1_speed;
    float cloud2_speed;
};
#endif
