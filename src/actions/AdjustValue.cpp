#include "AdjustValue.hpp"
#include "GuiManager.hpp"
#include "SoundManager.hpp"
#include "Menu.hpp"
#include "Label.hpp"

using gui::AdjustValue;
using isomot::SoundManager;

AdjustValue::AdjustValue(Menu* menu, const std::string& text, int value)
: menu(menu),
  text(text),
  value(value)
{

}

void AdjustValue::execute()
{
  std::stringstream ss;
  int pressedKey = KEY_MAX;

  ss << value;
  Label* label = new Label(this->text + ss.str());
  label->changeFont("big", "yellow");
  menu->changeOption(label->getText(), label);
  GuiManager::getInstance()->refresh();
  clear_keybuf();

  while(pressedKey != KEY_ESC && pressedKey != KEY_ENTER && pressedKey != KEY_UP && pressedKey != KEY_DOWN)
  {
    if(keypressed())
    {
      // Tecla pulsada por el usuario
      pressedKey = readkey() >> 8;

      switch(pressedKey)
      {
        case KEY_LEFT:
        case KEY_RIGHT:
          {
            // Si se pulsa el cursor izquierdo se baja el volumen
            if(pressedKey == KEY_LEFT)
            {
              value = (value > 0 ? value - 1 : 0);
            }
            // Si se pulsa el cursor derecho se sube el volumen
            else
            {
              value = (value < 99 ? value + 1 : 99);
            }

            ss.str(std::string());
            ss << value;
            std::string previous(label->getText());
            label = new Label(this->text + ss.str());
            label->changeFont("big", "yellow");
            menu->changeOption(previous, label);
            GuiManager::getInstance()->refresh();
          }
          break;

        default:
          ;
      }
    }

    // No te comas la CPU
    sleep(20);
  }

  if(pressedKey == KEY_ENTER)
  {
    std::string previous(label->getText());
    label = new Label(previous);
    label->changeFont("big", "white");
    menu->changeOption(previous, label);
    GuiManager::getInstance()->refresh();
  }
  else
  {
    simulate_keypress(pressedKey << 8);
  }
}
