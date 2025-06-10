#include "AccountScene.hpp"
#include "AccountSystem/AccountSystem.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <string>

void AccountScene::Initialize()
{
    passHide = true;
    statusMsg = "";
    statusMsgTimer = 0.0f;

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    activeTextBox = nullptr;

    bool logged_in = accountManager.isLoggedIn();

    if (!logged_in) {
        AddNewObject(new Engine::Label("Log In / Register", "romulus.ttf", 128,
                                       halfW, (double)halfH / 2.5, 255, 255,
                                       255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label("     Name:", "romulus.ttf", 96,
                                       halfW * 0.35, (double)halfH * 0.8, 255,
                                       255, 255, 255, 0.0, 0.5));
        AddNewObject(new Engine::Label("Password:", "romulus.ttf", 96,
                                       halfW * 0.35, (double)halfH * 1.1, 255,
                                       255, 255, 255, 0.0, 0.5));

        nameTextBox =
            new Engine::TextBox(halfW * 0.9f, halfH * 0.8f - 40, 500, 80,
                                "romulus.ttf", 64, "Enter your name");
        passwordTextBox =
            new Engine::TextBox(halfW * 0.9f, halfH * 1.1f - 40, 500, 80,
                                "romulus.ttf", 64, "Enter password");

        passwordTextBox->SetPasswordMode(passHide);

        nameTextBox->SetOnTextChanged(
            [this](const std::string &text) { userName = text; });

        passwordTextBox->SetOnTextChanged(
            [this](const std::string &text) { userPass = text; });

        nameTextBox->SetOnFocusGained([this]() {
            if (activeTextBox && activeTextBox != nameTextBox) {
                activeTextBox->LoseFocus();
            }
            activeTextBox = nameTextBox;
        });

        passwordTextBox->SetOnFocusGained([this]() {
            if (activeTextBox && activeTextBox != passwordTextBox) {
                activeTextBox->LoseFocus();
            }
            activeTextBox = passwordTextBox;
        });

        AddNewObject(nameTextBox);
        AddNewObject(passwordTextBox);

        btn = new Engine::ImageButton("clickable/back_normal.png",
                                      "clickable/back_hover.png", 50, 50, 100,
                                      100);
        btn->SetOnClickCallback(std::bind(&AccountScene::BackOnClick, this));
        AddNewControlObject(btn);

        btn = new Engine::ImageButton("clickable/lock_normal.png",
                                      "clickable/lock_hover.png", halfW * 1.45,
                                      halfH * 1.1 - 40, 100, 100);
        btn->SetOnClickCallback(
            std::bind(&AccountScene::ToggleVisibility, this));
        AddNewControlObject(btn);

        btn = new Engine::ImageButton("clickable/tick_normal.png",
                                      "clickable/tick_hover.png", halfW - 100,
                                      h - 200, 150, 150);
        btn->SetOnClickCallback(std::bind(&AccountScene::SubmitOnClick, this));
        AddNewControlObject(btn);
    }
    else 
    {
        const PlayerData& player = accountManager.getPlayer();
        
        AddNewObject(new Engine::Label("Welcome back!", "romulus.ttf", 128, halfW,
                                       (double)halfH / 2.5, 255, 255, 255, 255, 0.5,
                                       0.5));
        
        AddNewObject(new Engine::Label("Player: " + player.name, "romulus.ttf", 96, halfW,
                                       (double)halfH * 0.7, 255, 255, 255, 255, 0.5,
                                       0.5));
        
        btn = new Engine::ImageButton("clickable/back_normal.png",
                                      "clickable/back_hover.png", 50, 50, 100, 100);
        btn->SetOnClickCallback(std::bind(&AccountScene::BackOnClick, this));
        AddNewControlObject(btn);
        
        btn = new Engine::ImageButton("clickable/quit_normal.png",
                                      "clickable/quit_hover.png", halfW - 100,
                                      h - 200, 150, 150);
        btn->SetOnClickCallback(std::bind(&AccountScene::LogoutOnClick, this));
        AddNewControlObject(btn);
        
        // Add logout label near the button
        AddNewObject(new Engine::Label("Logout", "romulus.ttf", 64, halfW,
                                       h - 250, 255, 255, 255, 255, 0.5,
                                       0.5));
    }
}

// update the text boxes
void AccountScene::Update(float deltaTime)
{
    IScene::Update(deltaTime);
    if (nameTextBox)
        nameTextBox->Update(deltaTime);
    if (passwordTextBox)
        passwordTextBox->Update(deltaTime);

    if (statusMsgTimer > 0.0f) {
        statusMsgTimer -= deltaTime;
    }
}

// listen to event
void AccountScene::OnMouseDown(int button, int mx, int my)
{
    IScene::OnMouseDown(button, mx, my);

    bool anyTextBoxClicked = false;

    if (nameTextBox && nameTextBox->HandleMouseClick(mx, my)) {
        anyTextBoxClicked = true;
    }
    else if (passwordTextBox && passwordTextBox->HandleMouseClick(mx, my)) {
        anyTextBoxClicked = true;
    }

    if (!anyTextBoxClicked && activeTextBox) {
        activeTextBox->LoseFocus();
        activeTextBox = nullptr;
    }
}

void AccountScene::OnKeyDown(int keyCode)
{
    IScene::OnKeyDown(keyCode);

    // allow A-Z and 0-9
    if (activeTextBox) {
        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_9) {
            OnCharInput(keyCode);
        }
        else
            activeTextBox->HandleKeyPress(keyCode);
    }
}

void AccountScene::OnCharInput(int unicodeChar)
{
    if (activeTextBox)
        activeTextBox->HandleCharInput(unicodeChar);
}

void AccountScene::Terminate()
{
    nameTextBox = nullptr;
    passwordTextBox = nullptr;
    activeTextBox = nullptr;

    IScene::Terminate();
}

void AccountScene::BackOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void AccountScene::SubmitOnClick(void)
{
    if (userName.empty() || userPass.empty()) {
        ShowStatusMsg("Please fill in both Name && Password!");
        return;
    }

    // check if exists in database
    // yes login and switch to start sscene
    // else if name there say wrong pass
    // else register for that guy
    bool login_ed = accountManager.login(userName, userPass);

    if (login_ed) {
        ShowStatusMsg("Login Successful! Welcome, back " + userName + "!");

        const PlayerData &player = accountManager.getPlayer();
        Engine::GameEngine::GetInstance().ChangeScene("start");
    }
    else {
        bool registered = accountManager.createAccount(userName, userPass);

        if (registered) {
            ShowStatusMsg("Account created successfully! Welcome, " + userName +
                          "!");
            const PlayerData &player = accountManager.getPlayer();
            Engine::GameEngine::GetInstance().ChangeScene("start");
        }
        else {
            ShowStatusMsg("Wrong password :\(");
        }
    }

    Engine::GameEngine::GetInstance().ChangeScene("account");
}

void AccountScene::ToggleVisibility(void)
{
    passHide = !passHide;
    passwordTextBox->SetPasswordMode(passHide);
}

void AccountScene::ShowStatusMsg(const std::string &msg, float duration)
{
    statusMsg = msg;
    statusMsgTimer = duration;

    Engine::LOG(Engine::INFO) << "Status: " << msg;
}

void AccountScene::LogoutOnClick(void)
{
    // clear the buffer and perform logout
    accountManager.logout();
    userName = userPass = "";
    Engine::GameEngine::GetInstance().ChangeScene("account");
}
