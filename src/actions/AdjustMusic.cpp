#include "AdjustMusic.hpp"
#include "GuiManager.hpp"
#include "ConfigurationManager.hpp"
#include "SoundManager.hpp"
#include "Menu.hpp"

using gui::AdjustMusic;
using gui::GuiManager;
using isomot::SoundManager;

AdjustMusic::AdjustMusic(Menu* menu, const std::string& text)
: AdjustValue(menu, text, SoundManager::getInstance()->getMusicVolume())
{

}

void AdjustMusic::execute()
{
  AdjustValue::execute();
  SoundManager::getInstance()->setMusicVolume(value);
  GuiManager::getInstance()->getConfigurationManager()->setMusicVolume(value);
}
