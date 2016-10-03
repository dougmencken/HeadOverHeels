#include "CreateAudioMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "AdjustSoundFx.hpp"
#include "AdjustMusic.hpp"

using gui::CreateAudioMenu;
using isomot::SoundManager;

CreateAudioMenu::CreateAudioMenu(BITMAP* destination)
: Action(),
  destination(destination)
{

}

void CreateAudioMenu::execute()
{
  std::stringstream ss;
  Label* label = 0;
  LanguageText* langString = 0;
  Screen* screen = new Screen(0, 0, this->destination);
  LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

  // Imagen de fondo
  screen->setBackground(GuiManager::getInstance()->findImage("background"));
  screen->setAction(new CreateMainMenu(this->destination));

  // Etiqueta fija: JON
  langString = languageManager->findLanguageString("jon");
  label = new Label(langString->getX(), langString->getY(), langString->getText(), "regular", "multicolor");
  screen->addWidget(label);
  // Etiqueta fija: RITMAN
  langString = languageManager->findLanguageString("ritman");
  label = new Label(langString->getX(), langString->getY(), langString->getText(), "regular", "multicolor");
  screen->addWidget(label);
  // Etiqueta fija: BERNIE
  langString = languageManager->findLanguageString("bernie");
  label = new Label(langString->getX(), langString->getY(), langString->getText(), "regular", "multicolor");
  screen->addWidget(label);
  // Etiqueta fija: DRUMMOND
  langString = languageManager->findLanguageString("drummond");
  label = new Label(langString->getX(), langString->getY(), langString->getText(), "regular", "multicolor");
  screen->addWidget(label);

  // Etiqueta fija: HEAD
  label = new Label(200, 24, "HEAD");
  label->changeFont("big", "yellow");
  screen->addWidget(label);
  // Etiqueta fija: OVER
  label = new Label(280, 45, "OVER", "regular", "multicolor");
  screen->addWidget(label);
  // Etiqueta fija: HEELS
  label = new Label(360, 24, "HEELS");
  label->changeFont("big", "yellow");
  screen->addWidget(label);

  // 1. Efectos sonoros
  ss << SoundManager::getInstance()->getFxVolume();
  langString = languageManager->findLanguageString("soundfx");
  Menu* menu = new Menu(langString->getX(), langString->getY());
  label = new Label(langString->getText() + ss.str());
  label->setAction(new AdjustSoundFx(menu, languageManager->findLanguageString("soundfx")->getText()));
  menu->addActiveOption(label);

  // 2. Música
  ss.str(std::string());
  ss << SoundManager::getInstance()->getMusicVolume();
  label = new Label(languageManager->findLanguageString("music")->getText() + ss.str());
  label->setAction(new AdjustMusic(menu, languageManager->findLanguageString("music")->getText()));
  menu->addOption(label);
  screen->addWidget(menu);

  // Etiqueta fija: (C) 1987 OCEAN SOFTWARE LTD
  langString = languageManager->findLanguageString("ocean");
  label = new Label(langString->getX(), langString->getY(), langString->getText());
  label->changeFont("regular", "cyan");
  screen->addWidget(label);

  // Etiqueta fija: (C) 2008 JORGE RODRÍGUEZ SANTOS
  langString = languageManager->findLanguageString("jorge");
  label = new Label(langString->getX(), langString->getY(), langString->getText());
  label->changeFont("regular", "orange");
  screen->addWidget(label);

  // Crea la cadena de responsabilidad
  screen->setSucessor(menu);

  // Cambia la pantalla mostrada en la interfaz
  GuiManager::getInstance()->changeScreen(screen);
}
