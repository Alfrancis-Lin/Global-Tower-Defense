#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <string>
#include <functional>
#include <allegro5/allegro.h>
#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"

namespace Engine {
    class TextBox : public IObject {
    private:
        std::string text;
        std::string placeholder;
        float x, y, width, height;
        int fontSize;
        std::string fontName;
        bool isFocused;
        bool isActive;
        int maxLength;
        int cursorPosition;
        double cursorBlinkTime;
        bool showCursor;
        ALLEGRO_COLOR textColor;
        ALLEGRO_COLOR backgroundColor;
        ALLEGRO_COLOR borderColor;
        ALLEGRO_COLOR focusedBorderColor;
        ALLEGRO_COLOR placeholderColor;
        float borderWidth;
        bool isPassword;
        std::string displayText;
        
        std::function<void(const std::string&)> onTextChanged;
        std::function<void()> onEnterPressed;
        std::function<void()> onFocusGained;
        std::function<void()> onFocusLost;

    public:
        TextBox(float x, float y, float width, float height, 
                const std::string& fontName = "romulus.ttf", int fontSize = 24,
                const std::string& placeholder = "");
        
        virtual ~TextBox() = default;

        // Core methods
        void Update(float deltaTime) override;
        void Draw() const override;
        
        // Input handling
        bool HandleMouseClick(float mouseX, float mouseY);
        bool HandleKeyPress(int keycode);
        bool HandleCharInput(int unicodeChar);
        
        // Focus management
        void SetFocus(bool focused);
        bool IsFocused() const { return isFocused; }
        void LoseFocus();
        
        // Text management
        void SetText(const std::string& newText);
        std::string GetText() const { return text; }
        void SetPlaceholder(const std::string& newPlaceholder) { placeholder = newPlaceholder; }
        void SetMaxLength(int length) { maxLength = length; }
        
        // Password mode
        void SetPasswordMode(bool isPassword) { this->isPassword = isPassword; UpdateDisplayText(); }
        bool IsPasswordMode() const { return isPassword; }
        
        // Styling
        void SetTextColor(ALLEGRO_COLOR color) { textColor = color; }
        void SetBackgroundColor(ALLEGRO_COLOR color) { backgroundColor = color; }
        void SetBorderColor(ALLEGRO_COLOR normal, ALLEGRO_COLOR focused) { 
            borderColor = normal; 
            focusedBorderColor = focused; 
        }
        void SetBorderWidth(float width) { borderWidth = width; }
        
        // Callbacks
        void SetOnTextChanged(std::function<void(const std::string&)> callback) { onTextChanged = callback; }
        void SetOnEnterPressed(std::function<void()> callback) { onEnterPressed = callback; }
        void SetOnFocusGained(std::function<void()> callback) { onFocusGained = callback; }
        void SetOnFocusLost(std::function<void()> callback) { onFocusLost = callback; }
        
        // Utility
        bool IsPointInside(float pointX, float pointY) const;
        
    private:
        void UpdateDisplayText();
        void MoveCursor(int direction);
        void InsertCharAtCursor(char c);
        void DeleteCharAtCursor();
        void DeleteCharBeforeCursor();
    };
}

#endif // TEXTBOX_H
