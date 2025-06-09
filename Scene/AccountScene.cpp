#include "AccountScene.hpp"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void AccountScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    
    activeTextBox = nullptr;
    
    AddNewObject(new Engine::Label("Log In / Register", "romulus.ttf", 128, halfW,
                                   (double)halfH / 2.5, 255, 255, 255, 255, 0.5,
                                   0.5));
    AddNewObject(new Engine::Label("     Name:", "romulus.ttf", 96, halfW * 0.35,
                                   (double)halfH * 0.8, 255, 255, 255, 255, 0.0,
                                   0.5));
    AddNewObject(new Engine::Label("     Email:", "romulus.ttf", 96, halfW * 0.35 - 5,
                                   (double)halfH * 1.05, 255, 255, 255, 255, 0.0,
                                   0.5));
    AddNewObject(new Engine::Label("Password:", "romulus.ttf", 96, halfW * 0.35,
                                   (double)halfH * 1.3, 255, 255, 255, 255, 0.0,
                                   0.5));

    nameTextBox = new Engine::TextBox(halfW * 0.9f, halfH * 0.8f - 40, 500, 80, 
                                      "romulus.ttf", 64, "Enter your name");
    emailTextBox = new Engine::TextBox(halfW * 0.9f, halfH * 1.05f - 40, 500, 80, 
                                       "romulus.ttf", 64, "Enter your email");
    passwordTextBox = new Engine::TextBox(halfW * 0.9f, halfH * 1.3f - 40, 500, 80, 
                                          "romulus.ttf", 64, "Enter password");
    
    passwordTextBox->SetPasswordMode(true);
    
    nameTextBox->SetOnTextChanged([this](const std::string& text) {
        userName = text;
    });
    
    emailTextBox->SetOnTextChanged([this](const std::string& text) {
        userEmail = text;
    });
    
    passwordTextBox->SetOnTextChanged([this](const std::string& text) {
        userPassword = text;
    });
    
    nameTextBox->SetOnFocusGained([this]() {
        if (activeTextBox && activeTextBox != nameTextBox) {
            activeTextBox->LoseFocus();
        }
        activeTextBox = nameTextBox;
    });
    
    emailTextBox->SetOnFocusGained([this]() {
        if (activeTextBox && activeTextBox != emailTextBox) {
            activeTextBox->LoseFocus();
        }
        activeTextBox = emailTextBox;
    });
    
    passwordTextBox->SetOnFocusGained([this]() {
        if (activeTextBox && activeTextBox != passwordTextBox) {
            activeTextBox->LoseFocus();
        }
        activeTextBox = passwordTextBox;
    });
    
    AddNewObject(nameTextBox);
    AddNewObject(emailTextBox);
    AddNewObject(passwordTextBox);
    
    btn = new Engine::ImageButton("clickable/back_normal.png",
                                  "clickable/back_hover.png", 50, 50, 100, 100);
    btn->SetOnClickCallback(std::bind(&AccountScene::BackOnClick, this));
    AddNewControlObject(btn);
    
    // Submit button (changed callback to handle form submission)
    btn = new Engine::ImageButton("clickable/tick_normal.png",
                                  "clickable/tick_hover.png", halfW - 100,
                                  h - 200, 150, 150);
    btn->SetOnClickCallback(std::bind(&AccountScene::SubmitOnClick, this));
    AddNewControlObject(btn);
}

void AccountScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    
    if (nameTextBox) nameTextBox->Update(deltaTime);
    if (emailTextBox) emailTextBox->Update(deltaTime);
    if (passwordTextBox) passwordTextBox->Update(deltaTime);
}

// Add these event handlers to your AccountScene class:
void AccountScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    
    // Handle textbox clicks
    bool anyTextBoxClicked = false;
    
    if (nameTextBox && nameTextBox->HandleMouseClick(mx, my)) {
        anyTextBoxClicked = true;
    } else if (emailTextBox && emailTextBox->HandleMouseClick(mx, my)) {
        anyTextBoxClicked = true;
    } else if (passwordTextBox && passwordTextBox->HandleMouseClick(mx, my)) {
        anyTextBoxClicked = true;
    }
    
    // If clicked outside all textboxes, remove focus from active textbox
    if (!anyTextBoxClicked && activeTextBox) {
        activeTextBox->LoseFocus();
        activeTextBox = nullptr;
    }
}

void AccountScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    
    if (activeTextBox) {
        activeTextBox->HandleKeyPress(keyCode);
    }
}

void AccountScene::OnCharInput(int unicodeChar) {
    // Handle character input for active textbox
    if (activeTextBox) 
    {
        Engine::LOG(Engine::INFO) << unicodeChar;
        activeTextBox->HandleCharInput(unicodeChar);
    }
}

void AccountScene::Terminate() { 
    // Clean up
    nameTextBox = nullptr;
    emailTextBox = nullptr;
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
    // Handle form submission
    if (userName.empty() || userEmail.empty() || userPassword.empty()) {
        // Show error message or highlight empty fields
        // You could add a label to show error messages
        return;
    }
    
    // Process login/registration here
    // For now, just go back to start scene
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
