#include "GuiManager.hpp"
#include "Ism.hpp"
#include "FontManager.hpp"
#include "ConfigurationManager.hpp"
#include "LanguageManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Icon.hpp"
#include "Label.hpp"
#include "actions/CreateMainMenu.hpp"
#include "actions/CreateLanguageMenu.hpp"

using gui::GuiManager;
using gui::LanguageManager;
using gui::ConfigurationManager;
using isomot::InputManager;
using isomot::SoundManager;

GuiManager* GuiManager::instance = 0;

GuiManager::GuiManager()
: screen(0),
  active(true)
{
  // Inicialización de Allegro
  this->allegroSetup();

  // Título de la ventana
  set_window_title("Head over Heels v1.0.1");

  // Ruta absoluta a los datos
  std::string path = isomot::sharePath();

  BITMAP* bitmap = 0;
  // Carga y almacena las imágenes
  bitmap = load_bitmap((path + "gfx/gui/background.bmp").c_str(), 0);
  assert(bitmap);
  images["background"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/blacktooth.bmp").c_str(), 0);
  assert(bitmap);
  images["blacktooth"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/byblos.bmp").c_str(), 0);
  assert(bitmap);
  images["byblos"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/crown.bmp").c_str(), 0);
  assert(bitmap);
  images["crown"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/donuts.bmp").c_str(), 0);
  assert(bitmap);
  images["donuts"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/egyptus.bmp").c_str(), 0);
  assert(bitmap);
  images["egyptus"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/grey-crown.bmp").c_str(), 0);
  assert(bitmap);
  images["grey-crown"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/grey-donuts.bmp").c_str(), 0);
  assert(bitmap);
  images["grey-donuts"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/grey-handbag.bmp").c_str(), 0);
  assert(bitmap);
  images["grey-handbag"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/grey-head.bmp").c_str(), 0);
  assert(bitmap);
  images["grey-head"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/grey-heels.bmp").c_str(), 0);
  assert(bitmap);
  images["grey-heels"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/grey-horn.bmp").c_str(), 0);
  assert(bitmap);
  images["grey-horn"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/handbag.bmp").c_str(), 0);
  assert(bitmap);
  images["handbag"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/head.bmp").c_str(), 0);
  assert(bitmap);
  images["head"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/heels.bmp").c_str(), 0);
  assert(bitmap);
  images["heels"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/horn.bmp").c_str(), 0);
  assert(bitmap);
  images["horn"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/penitentiary.bmp").c_str(), 0);
  assert(bitmap);
  images["penitentiary"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/safari.bmp").c_str(), 0);
  assert(bitmap);
  images["safari"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/option.bmp").c_str(), 0);
  assert(bitmap);
  images["option"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/selected-option.bmp").c_str(), 0);
  assert(bitmap);
  images["selected-option"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/selected-option-mini.bmp").c_str(), 0);
  assert(bitmap);
  images["selected-option-mini"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/jail-frame.bmp").c_str(), 0);
  assert(bitmap);
  images["jail-frame"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/blacktooth-frame.bmp").c_str(), 0);
  assert(bitmap);
  images["blacktooth-frame"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/market-frame.bmp").c_str(), 0);
  assert(bitmap);
  images["market-frame"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/moonbase-frame.bmp").c_str(), 0);
  assert(bitmap);
  images["moonbase-frame"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/egyptus-frame.bmp").c_str(), 0);
  assert(bitmap);
  images["egyptus-frame"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/penitentiary-frame.bmp").c_str(), 0);
  assert(bitmap);
  images["penitentiary-frame"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/safari-frame.bmp").c_str(), 0);
  assert(bitmap);
  images["safari-frame"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/byblos-frame.bmp").c_str(), 0);
  assert(bitmap);
  images["byblos-frame"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/shield.bmp").c_str(), 0);
  assert(bitmap);
  images["shield"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/high-jumps.bmp").c_str(), 0);
  assert(bitmap);
  images["high-jumps"] = bitmap;
  bitmap = load_bitmap((path + "gfx/gui/high-speed.bmp").c_str(), 0);
  assert(bitmap);
  images["high-speed"] = bitmap;

  // Se crean, a través del gesto de fuentes, todos los tipos de letra empleados en el juego
  gui::FontManager::getInstance()->createFont("regular-white", path + "gfx/gui/font.bmp", makecol(255, 255, 255));
  gui::FontManager::getInstance()->createFont("big-white", path + "gfx/gui/font.bmp", makecol(255, 255, 255), true);
  gui::FontManager::getInstance()->createFont("regular-orange", path + "gfx/gui/font.bmp", makecol(239, 129, 0));
  gui::FontManager::getInstance()->createFont("regular-cyan", path + "gfx/gui/font.bmp", makecol(0, 228, 231));
  gui::FontManager::getInstance()->createFont("regular-yellow", path + "gfx/gui/font.bmp", makecol(255, 255, 50));
  gui::FontManager::getInstance()->createFont("big-orange", path + "gfx/gui/font.bmp", makecol(239, 129, 0), true);
  gui::FontManager::getInstance()->createFont("big-cyan", path + "gfx/gui/font.bmp", makecol(0, 228, 231), true);
  gui::FontManager::getInstance()->createFont("big-yellow", path + "gfx/gui/font.bmp", makecol(255, 255, 50), true);

  // Se crea la imagen donde se dibujará la interfaz gráfica
  this->destination = create_bitmap_ex(32, 640, 480);

  // Se crea el gestor de configuración y se cargan los datos del disco
  this->configurationManager = new ConfigurationManager(isomot::homePath() + "configuration.xml");
  this->configurationManager->read();

  // No hay lengua definida todavía
  this->languageManager = 0;

  // Se comunica al gestor de entradas qué teclas
  InputManager* inputManager = InputManager::getInstance();
  isomot::GameKey gameKey[] = { isomot::KeyNorth, isomot::KeySouth, isomot::KeyEast, isomot::KeyWest,
                                isomot::KeyTake, isomot::KeyJump, isomot::KeyShoot, isomot::KeyTakeAndJump,
                                isomot::KeySwap, isomot::KeyHalt };
  for(int i = 0; i < 10; i++)
  {
    inputManager->changeUserKey(gameKey[i], configurationManager->getKey(gameKey[i]));
  }

  // Se inicializa el gestor de sonido cargando los sonidos
  SoundManager::getInstance()->load("sounds.xml");

  // Se comunica al gestor de sonido los niveles del volumen
  SoundManager::getInstance()->setFxVolume(configurationManager->getSoundFxVolume());
  SoundManager::getInstance()->setMusicVolume(configurationManager->getMusicVolume());
}

GuiManager::~GuiManager()
{
  for(std::map<std::string, BITMAP*>::iterator i = this->images.begin(); i != this->images.end(); ++i)
  {
    delete (*i).second;
  }

  this->images.clear();

  delete this->languageManager;
  delete this->configurationManager;
}

GuiManager* GuiManager::getInstance()
{
  if(instance == 0)
  {
    instance = new GuiManager();
  }

  return instance;
}

void GuiManager::start()
{
  // Se presenta la lista de lenguas disponibles
  std::auto_ptr<CreateLanguageMenu> languageMenu(new CreateLanguageMenu(this->destination));
  languageMenu->execute();

  // Dibuja la interfaz de usuario y procesa la pulsación de teclas
  while(this->active)
  {
    this->screen->draw(this->destination);

    if(keypressed())
    {
      this->screen->handleKey(readkey());
    }

    sleep(30);
  }
}

void GuiManager::changeScreen(Screen* screen)
{
  if(this->screen != 0)
  {
    delete this->screen;
  }

  this->screen = screen;
}

void GuiManager::refresh()
{
  if(this->active)
  {
    this->screen->draw(this->destination);
  }
}

BITMAP* GuiManager::findImage(const std::string& name)
{
  std::map<std::string, BITMAP*>::iterator i  = images.find(name);
  return (i != images.end() ? (*i).second : 0);
}

void GuiManager::assignLanguage(const std::string& language)
{
  if(this->languageManager == 0)
  {
    this->languageManager = new LanguageManager(isomot::sharePath() + "text/" + language + ".xml");
  }
}

void GuiManager::allegroSetup()
{
  // Inicializa Allegro
  allegro_init();
  // Trabajo a color real con canal alfa
  set_color_depth(32);
  // Resolución de 640x480 píxeles en ventana
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
#ifdef __WIN32
  // Si la aplicación pierde el foco el juego se seguirá ejecutando
  // Se opera de este modo porque hay subprocesos que seguirán ejecutándose aunque el
  // subproceso principal se pare
  set_display_switch_mode(SWITCH_BACKGROUND);
#endif
  // Instala el controlador de eventos del teclado
  install_keyboard();
}

ConfigurationManager* GuiManager::getConfigurationManager() const
{
  return this->configurationManager;
}

LanguageManager* GuiManager::getLanguageManager() const
{
  return this->languageManager;
}

