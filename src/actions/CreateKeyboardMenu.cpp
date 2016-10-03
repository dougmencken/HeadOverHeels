#include "CreateKeyboardMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "InputManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "RedefineKey.hpp"

using gui::CreateKeyboardMenu;
using isomot::InputManager;

CreateKeyboardMenu::CreateKeyboardMenu(BITMAP* destination)
: Action(),
  destination(destination)
{

}

void CreateKeyboardMenu::execute()
{
  Label* label = 0;
  LanguageText* langString = 0;
  Screen* screen = new Screen(0, 0, this->destination);
  LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();
  InputManager* inputManager = InputManager::getInstance();

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

  // Las opciones del menú
  std::string textKey[] = { "left", "right", "up", "down", "take", "jump", "shoot", "take-jump", "swap", "halt" };
  isomot::GameKey gameKey[] = { isomot::KeyNorth, isomot::KeySouth, isomot::KeyEast, isomot::KeyWest,
                                isomot::KeyTake, isomot::KeyJump, isomot::KeyShoot, isomot::KeyTakeAndJump,
                                isomot::KeySwap, isomot::KeyHalt };

  // La primera opción almacena la posición del menú en pantalla
  langString = languageManager->findLanguageString("left");
  Menu* menu = new Menu(langString->getX(), langString->getY());

  // Creación de las opciones: una por cada tecla empleada en el juego
  for(int i = 0; i < 10; i++)
  {
    // Código de la tecla asignada
    int scancode = inputManager->readUserKey(gameKey[i]);
    // Descripción del uso de la tecla
    std::string keyText = languageManager->findLanguageString(textKey[i])->getText();
    // Se crea la opción de menú con la descripción de la tecla y la propia tecla
    label = new Label(keyText + scancode_to_name(scancode));
    // Se asigna como acción el cambio de la tecla por parte del usuario
    label->setAction(new RedefineKey(menu, keyText, scancode));
    // La primera opción es la opción activa
    if(i == 0)
    {
      menu->addActiveOption(label);
    }
    else
    {
      menu->addOption(label);
    }
  }

  // El menú que aparece en pantalla
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
