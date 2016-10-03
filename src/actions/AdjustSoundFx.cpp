#include "AdjustSoundFx.hpp"
#include "GuiManager.hpp"
#include "ConfigurationManager.hpp"
#include "SoundManager.hpp"
#include "Menu.hpp"

using gui::AdjustSoundFx;
using gui::GuiManager;
using isomot::SoundManager;

AdjustSoundFx::AdjustSoundFx(Menu* menu, const std::string& text)
: AdjustValue(menu, text, SoundManager::getInstance()->getFxVolume())
{

}

void AdjustSoundFx::execute()
{
  AdjustValue::execute();
  SoundManager::getInstance()->setFxVolume(value);
  GuiManager::getInstance()->getConfigurationManager()->setSoundFxVolume(value);
}
