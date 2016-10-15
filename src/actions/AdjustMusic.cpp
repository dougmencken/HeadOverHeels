#include "AdjustMusic.hpp"
#include "GuiManager.hpp"
#include "ConfigurationManager.hpp"
#include "SoundManager.hpp"
#include "Menu.hpp"

using gui::AdjustMusic;
using gui::GuiManager;
using isomot::SoundManager;

AdjustMusic::AdjustMusic( Menu* menu, const std::string& text )
: AdjustValue( menu, text, SoundManager::getInstance()->getVolumeOfMusic() )
{

}

void AdjustMusic::doIt ()
{
        AdjustValue::doIt ();
        SoundManager::getInstance()->setVolumeOfMusic( value );
}
