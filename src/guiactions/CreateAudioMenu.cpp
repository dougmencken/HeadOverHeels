
#include "CreateAudioMenu.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"
#include "Screen.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

#include "sleep.hpp"

#include <string>
#include <sstream>

using gui::CreateAudioMenu;
using iso::SoundManager;


CreateAudioMenu::CreateAudioMenu( ) :
        Action( ),
        listOfOptions( nilPointer ),
        labelEffects( nilPointer ),
        labelMusic( nilPointer ),
        playRoomTunes( nilPointer )
{

}

void CreateAudioMenu::doAction ()
{
        LanguageManager* languageManager = GuiManager::getInstance().getLanguageManager();

        LanguageText* langStringEffects = languageManager->findLanguageStringForAlias( "soundfx" );
        LanguageText* langStringMusic = languageManager->findLanguageStringForAlias( "music" );

        LanguageText* langStringRoomTunes = languageManager->findLanguageStringForAlias( "play-room-melodies" );

        std::string yeah = languageManager->findLanguageStringForAlias( "yep" )-> getText ();
        std::string nope = languageManager->findLanguageStringForAlias( "nope" )->getText ();

        std::stringstream ss;

        Screen& screen = * GuiManager::getInstance().findOrCreateScreenForAction( this );
        if ( screen.countWidgets() == 0 )
        {
                screen.setEscapeAction( new CreateMainMenu() );

                screen.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                // efectos sonoros
                this->labelEffects = new Label( langStringEffects->getText() );

                // música
                this->labelMusic = new Label( langStringMusic->getText() );

                // play melody of scenery on entry to room or don’t
                this->playRoomTunes = new Label( langStringRoomTunes->getText() );

                // create menu

                this->listOfOptions = new MenuWithValues( );

                listOfOptions->addOption( labelEffects );
                ss.str( std::string() ); // clear ss
                ss << SoundManager::getInstance().getVolumeOfEffects();
                listOfOptions->setValueOf( labelEffects, ss.str() );

                listOfOptions->addOption( labelMusic );
                ss.str( std::string() );
                ss << SoundManager::getInstance().getVolumeOfMusic();
                listOfOptions->setValueOf( labelMusic, ss.str() );

                listOfOptions->addOption( playRoomTunes );
                listOfOptions->setValueOf( playRoomTunes, game::GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );

                listOfOptions->setVerticalOffset( 35 );

                // add menu to screen

                screen.addWidget( listOfOptions );
        }
        else
        {
                // update labels
                listOfOptions->setValueOf( playRoomTunes, game::GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );
        }

        if ( screen.getKeyHandler() == nilPointer )
        {
                screen.setKeyHandler( listOfOptions );
        }

        GuiManager::getInstance().changeScreen( screen, true );

        allegro::emptyKeyboardBuffer();

        while ( true )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        // get the key pressed by user
                        std::string theKey = allegro::nextKey() ;

                        if ( theKey == "Escape" )
                        {
                                allegro::emptyKeyboardBuffer();
                                screen.handleKey ( theKey );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( listOfOptions->getActiveOption () == labelMusic || listOfOptions->getActiveOption () == labelEffects )
                                {
                                        int musicVolume = SoundManager::getInstance().getVolumeOfMusic();
                                        int effectsVolume = SoundManager::getInstance().getVolumeOfEffects();
                                        int value = ( listOfOptions->getActiveOption () == labelMusic ) ? musicVolume : effectsVolume ;
                                        int previousValue = value;

                                        if ( theKey == "Left" || theKey == "o" )
                                        {
                                                value = ( value > 0 ? value - 1 : 0 ); // decrease volume
                                                doneWithKey = true;
                                        }
                                        else if ( theKey == "Right" || theKey == "p" )
                                        {
                                                value = ( value < 99 ? value + 1 : 99 ); // increase volume
                                                doneWithKey = true;
                                        }

                                        if ( value != previousValue )
                                        {
                                                ss.str( std::string() ); // clear ss
                                                ss << value;

                                                if ( listOfOptions->getActiveOption () == labelMusic )
                                                {
                                                        listOfOptions->setValueOf( labelMusic, ss.str() );
                                                        SoundManager::getInstance().setVolumeOfMusic( value );
                                                }
                                                else if ( listOfOptions->getActiveOption () == labelEffects )
                                                {
                                                        listOfOptions->setValueOf( labelEffects, ss.str() );
                                                        SoundManager::getInstance().setVolumeOfEffects( value );
                                                        iso::SoundManager::getInstance().stopEverySound ();
                                                        iso::SoundManager::getInstance().play ( "gui", iso::Activity::Push, /* loop */ false );
                                                }
                                        }
                                }
                                else if ( listOfOptions->getActiveOption () == playRoomTunes )
                                {
                                        if ( theKey == "Left" || theKey == "Right" || theKey == "o" || theKey == "p" )
                                        {
                                                game::GameManager::getInstance().togglePlayMelodyOfScenery ();
                                                listOfOptions->setValueOf( playRoomTunes, game::GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );

                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                {
                                        screen.getKeyHandler()->handleKey ( theKey );
                                }

                                allegro::emptyKeyboardBuffer();
                                listOfOptions->redraw ();
                        }
                }

                // no te comas la CPU
                // do not eat the CPU
                somn::milliSleep( 25 );
        }
}
