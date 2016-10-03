#include "ShowAuthors.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Label.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"

using gui::ShowAuthors;
using gui::Label;
using gui::TextField;
using isomot::SoundManager;

ShowAuthors::ShowAuthors(BITMAP* destination)
: Action(),
  destination(destination)
{

}

void ShowAuthors::execute()
{
  // Detiene la reproducción del tema principal
  SoundManager::getInstance()->stopOgg();
  // Inicia la reproducción del tema "Head over Heels"
  SoundManager::getInstance()->playOgg("music/CreditsTheme.ogg");

  LanguageText* langString = 0;
  Screen* screen = new Screen(0, 0, this->destination);
  LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

  // Imagen de fondo
  screen->setBackground(GuiManager::getInstance()->findImage("background"));
  screen->setAction(new CreateMainMenu(this->destination));

  langString = languageManager->findLanguageString("credits-text");
  TextField* textField = new TextField(langString->getX(), langString->getY(), 640, 480, CenterAlignment);

  for(size_t i = 0; i < langString->getLinesCount(); i++)
  {
    LanguageLine* line = langString->getLine(i);
    textField->addLine(line->text, line->font, line->color);
  }

  screen->addWidget(textField);

  // Cambia la pantalla mostrada en la interfaz
  GuiManager::getInstance()->changeScreen(screen);

  // Mientras no se pulse Escape, se mueve el texto hacia arriba
  bool exit = false;
  while(!exit)
  {
    textField->changePosition(textField->getX(), textField->getY() - 1);
    GuiManager::getInstance()->refresh();
    exit = (keypressed() && key[KEY_ESC]);
    sleep(50);
  }
}


