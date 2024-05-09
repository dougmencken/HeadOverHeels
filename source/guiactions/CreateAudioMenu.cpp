
#include "CreateAudioMenu.hpp"

#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageStrings.hpp"
#include "SoundManager.hpp"
#include "Screen.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "CreateOptionsMenu.hpp"

#include "sleep.hpp"

#include <string>
#include <sstream>

using gui::CreateAudioMenu ;


CreateAudioMenu::CreateAudioMenu( ) :
        Action( ),
        listOfOptions( nilPointer ),
        labelEffects( nilPointer ),
        labelMusic( nilPointer ),
        playRoomTunes( nilPointer )
{

}

void CreateAudioMenu::act ()
{
        LanguageStrings* languageStrings = GuiManager::getInstance().getLanguageStrings() ;

        LanguageText* langStringEffects = languageStrings->getTranslatedTextByAlias( "soundfx" );
        LanguageText* langStringMusic = languageStrings->getTranslatedTextByAlias( "music" );

        LanguageText* langStringRoomTunes = languageStrings->getTranslatedTextByAlias( "play-room-melodies" );

        std::string yeah = languageStrings->getTranslatedTextByAlias( "yep" )-> getText ();
        std::string nope = languageStrings->getTranslatedTextByAlias( "nope" )->getText ();

        std::stringstream ss;

        Screen & screen = * GuiManager::getInstance().findOrCreateScreenForAction( *this );
        if ( screen.isNewAndEmpty() )
        {
                screen.setEscapeAction( new CreateOptionsMenu() );

                screen.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                // efectos sonoros
                this->labelEffects = new Label( langStringEffects->getText() );

                // música
                this->labelMusic = new Label( langStringMusic->getText() );

                // play the melody of scenery on entry to a room or don’t
                this->playRoomTunes = new Label( langStringRoomTunes->getText() );

                // create the menu

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
                listOfOptions->setValueOf( playRoomTunes, GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );

                listOfOptions->setVerticalOffset( 35 );

                // add this menu to the screen

                screen.addWidget( listOfOptions );
        }
        else
        {
                // update labels
                listOfOptions->setValueOf( playRoomTunes, GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );
        }

        if ( screen.getNextKeyHandler() == nilPointer )
                screen.setNextKeyHandler( listOfOptions );

        GuiManager::getInstance().changeScreen( screen, true );

        allegro::emptyKeyboardBuffer();

        while ( true )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        // get the key pressed by the user
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
                                                        SoundManager::getInstance().stopEverySound ();
                                                        SoundManager::getInstance().play ( "menus", "effect", /* loop */ false );
                                                }
                                        }
                                }
                                else if ( listOfOptions->getActiveOption () == playRoomTunes )
                                {
                                        if ( theKey == "Left" || theKey == "Right" || theKey == "o" || theKey == "p" )
                                        {
                                                GameManager::getInstance().togglePlayMelodyOfScenery ();
                                                listOfOptions->setValueOf( playRoomTunes, GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );

                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                        screen.getNextKeyHandler()->handleKey( theKey );

                                allegro::emptyKeyboardBuffer();
                                listOfOptions->redraw ();
                        }
                }

                // no te comas la CPU
                // do not eat the CPU
                somn::milliSleep( 25 );
        }
}
