#ifndef ACCOUNTSCENE_HPP
#define ACCOUNTSCENE_HPP

#include "UI/Component/TextBox.hpp"
#include <allegro5/allegro_audio.h>

#include "Engine/IScene.hpp"
class AccountScene final : public Engine::IScene {
  private:
    Engine::TextBox *nameTextBox;
    Engine::TextBox *emailTextBox;
    Engine::TextBox *passwordTextBox;
    Engine::TextBox *activeTextBox; // Currently focused textbox

    std::string userName;
    std::string userEmail;
    std::string userPassword;

  public:
    explicit AccountScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;

    void OnMouseDown(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnCharInput(int unicodeChar);

    void BackOnClick(void);
    void SubmitOnClick(void);
};
#endif
