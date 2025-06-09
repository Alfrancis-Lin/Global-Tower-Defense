#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "TextBox.hpp"
#include <algorithm>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <memory>

namespace Engine {

TextBox::TextBox(float x, float y, float width, float height,
                 const std::string &fontName, int fontSize,
                 const std::string &placeholder)
    : x(x), y(y), width(width), height(height), fontSize(fontSize),
      fontName(fontName), placeholder(placeholder), isFocused(false),
      isActive(true), maxLength(12), cursorPosition(0), cursorBlinkTime(0.0),
      showCursor(true), borderWidth(2.0f), isPassword(false)
{

    textColor = al_map_rgb(255, 255, 255);
    backgroundColor = al_map_rgba(40, 40, 40, 200);
    borderColor = al_map_rgb(100, 100, 100);
    focusedBorderColor = al_map_rgb(0, 150, 255);
    placeholderColor = al_map_rgb(150, 150, 150);
}

void TextBox::Update(float deltaTime)
{
    if (!isActive)
        return;

    if (isFocused) {
        cursorBlinkTime += deltaTime;
        if (cursorBlinkTime >= 0.5) {
            showCursor = !showCursor;
            cursorBlinkTime = 0.0;
        }
    }
}

void TextBox::Draw() const
{
    if (!isActive)
        return;

    // Draw background
    al_draw_filled_rectangle(x, y, x + width, y + height, backgroundColor);

    // Draw border
    ALLEGRO_COLOR currentBorderColor =
        isFocused ? focusedBorderColor : borderColor;
    al_draw_rectangle(x, y, x + width, y + height, currentBorderColor,
                      borderWidth);

    // Get font
    ALLEGRO_FONT *font =
        (Resources::GetInstance().GetFont(fontName, fontSize)).get();

    if (!font)
        return;

    // Calculate text position
    float textX = x + 10; // Left padding
    float textY = y + (height - al_get_font_line_height(font)) / 2;

    // Draw text or placeholder
    if (text.empty() && !isFocused) {
        al_draw_text(font, placeholderColor, textX, textY, 0,
                     placeholder.c_str());
    }
    else {
        const std::string &drawText = isPassword ? displayText : text;
        al_draw_text(font, textColor, textX, textY, 0, drawText.c_str());

        // Draw cursor if focused
        if (isFocused && showCursor) {
            // Calculate cursor position
            std::string beforeCursor = drawText.substr(0, cursorPosition);
            float cursorX =
                textX + al_get_text_width(font, beforeCursor.c_str());
            float cursorY1 = textY;
            float cursorY2 = textY + al_get_font_line_height(font);

            al_draw_line(cursorX, cursorY1, cursorX, cursorY2, textColor, 1.0f);
        }
    }
}

bool TextBox::HandleMouseClick(float mouseX, float mouseY)
{
    if (!isActive)
        return false;

    bool wasInside = IsPointInside(mouseX, mouseY);

    if (wasInside && !isFocused) {
        SetFocus(true);
        return true;
    }
    else if (!wasInside && isFocused) {
        SetFocus(false);
        return false;
    }

    return wasInside;
}

bool TextBox::HandleKeyPress(int keycode)
{
    if (!isFocused || !isActive)
        return false;

    switch (keycode) {
    case ALLEGRO_KEY_LEFT:
        MoveCursor(-1);
        return true;

    case ALLEGRO_KEY_RIGHT:
        MoveCursor(1);
        return true;

    case ALLEGRO_KEY_HOME:
        cursorPosition = 0;
        return true;

    case ALLEGRO_KEY_END:
        cursorPosition = text.length();
        return true;

    case ALLEGRO_KEY_BACKSPACE:
        DeleteCharBeforeCursor();
        return true;

    case ALLEGRO_KEY_DELETE:
        DeleteCharAtCursor();
        return true;

    case ALLEGRO_KEY_ENTER:
        if (onEnterPressed) {
            onEnterPressed();
        }
        return true;

    default:
        return false;
    }
}

bool TextBox::HandleCharInput(int unicodeChar)
{
    if (!isFocused || !isActive)
        return false;

    if (unicodeChar >= ALLEGRO_KEY_A && unicodeChar <= 126)
    {
        if (text.length() < maxLength) {
            InsertCharAtCursor(static_cast<char>(unicodeChar));
            return true;
        }
    }

    return false;
}

void TextBox::SetFocus(bool focused)
{
    if (isFocused == focused)
        return;

    isFocused = focused;
    cursorBlinkTime = 0.0;
    showCursor = true;

    if (focused) {
        cursorPosition = text.length(); // Move cursor to end
        if (onFocusGained) {
            onFocusGained();
        }
    }
    else {
        if (onFocusLost) {
            onFocusLost();
        }
    }
}

void TextBox::LoseFocus() { SetFocus(false); }

void TextBox::SetText(const std::string &newText)
{
    text = newText.substr(0, maxLength);
    cursorPosition = std::min(cursorPosition, static_cast<int>(text.length()));
    UpdateDisplayText();

    if (onTextChanged) {
        onTextChanged(text);
    }
}

bool TextBox::IsPointInside(float pointX, float pointY) const
{
    return pointX >= x && pointX <= x + width && pointY >= y &&
           pointY <= y + height;
}

void TextBox::UpdateDisplayText()
{
    if (isPassword) {
        displayText = std::string(text.length(), '*');
    }
    else {
        displayText = text;
    }
}

void TextBox::MoveCursor(int direction)
{
    cursorPosition += direction;
    cursorPosition =
        std::max(0, std::min(cursorPosition, static_cast<int>(text.length())));
}

void TextBox::InsertCharAtCursor(char c)
{
    if (text.length() < maxLength) {
        text.insert(cursorPosition, 1, c);
        cursorPosition++;
        UpdateDisplayText();

        if (onTextChanged) {
            onTextChanged(text);
        }
    }
}

void TextBox::DeleteCharAtCursor()
{
    if (cursorPosition < text.length()) {
        text.erase(cursorPosition, 1);
        UpdateDisplayText();

        if (onTextChanged) {
            onTextChanged(text);
        }
    }
}

void TextBox::DeleteCharBeforeCursor()
{
    if (cursorPosition > 0) {
        text.erase(cursorPosition - 1, 1);
        cursorPosition--;
        UpdateDisplayText();

        if (onTextChanged) {
            onTextChanged(text);
        }
    }
}

} 
