#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Icon.hpp"
#include "Label.hpp"
#include "CreatePlanetsScreen.hpp"
#include "CreateKeyboardMenu.hpp"
#include "CreateAudioMenu.hpp"
#include "CreateFileGameMenu.hpp"
#include "ShowAuthors.hpp"
#include "ExitApplication.hpp"

using gui::CreateMainMenu;
using isomot::SoundManager;

CreateMainMenu::CreateMainMenu(BITMAP* destination)
: Action(),
  destination(destination)
{

}

void CreateMainMenu::execute()
{
  // Si no está sonando la melodía principal:
  if(!SoundManager::getInstance()->isPlayingTheme("music/MainTheme.ogg"))
  {
    // Detiene la reproducción del tema en curso
    SoundManager::getInstance()->stopOgg();
    // Se reproduce la melodía principal
    SoundManager::getInstance()->playOgg("music/MainTheme.ogg");
  }

  Label* label = 0;
  LanguageText* langString = 0;
  Screen* screen = new Screen(0, 0, this->destination);
  LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

  // Imagen de fondo
  screen->setBackground(GuiManager::getInstance()->findImage("background"));

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

  // Icono: Head
  screen->addWidget(new Icon(206, 84, GuiManager::getInstance()->findImage("head")));
  // Icono: Heels
  screen->addWidget(new Icon(378, 84, GuiManager::getInstance()->findImage("heels")));

  // Las opciones del menú principal
  // 1. Nuevo juego
  langString = languageManager->findLanguageString("new-game");
  Menu* menu = new Menu(langString->getX(), langString->getY());
  label = new Label(langString->getText());
  label->setAction(new CreatePlanetsScreen(this->destination, false));
  menu->addActiveOption(label);
  // 2. Cargar juego
  label = new Label(languageManager->findLanguageString("load-game")->getText());
  label->setAction(new CreateFileGameMenu(this->destination, true));
  menu->addOption(label);
  // 3. Teclado
  label = new Label(languageManager->findLanguageString("keyboard")->getText());
  label->setAction(new CreateKeyboardMenu(this->destination));
  menu->addOption(label);
  // 4. Sonido
  label = new Label(languageManager->findLanguageString("audio")->getText());
  label->setAction(new CreateAudioMenu(this->destination));
  menu->addOption(label);
  // 5. Autores
  label = new Label(languageManager->findLanguageString("credits")->getText());
  label->setAction(new ShowAuthors(this->destination));
  menu->addOption(label);
  // 6. Salir al escritorio
  label = new Label(languageManager->findLanguageString("exit")->getText());
  label->setAction(new ExitApplication());
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
