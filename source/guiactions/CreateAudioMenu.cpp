
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


void gui::CreateAudioMenu::act ()
{
        LanguageStrings* languageStrings = GuiManager::getInstance().getLanguageStrings() ;

        LanguageText* langStringEffects = languageStrings->getTranslatedTextByAlias( "soundfx" );
        LanguageText* langStringMusic = languageStrings->getTranslatedTextByAlias( "music" );

        LanguageText* langStringRoomTunes = languageStrings->getTranslatedTextByAlias( "play-room-melodies" );

        std::string yeah = languageStrings->getTranslatedTextByAlias( "yep" )-> getText ();
        std::string nope = languageStrings->getTranslatedTextByAlias( "nope" )->getText ();

        std::stringstream ss;

        Screen & slideWithAudioMenu = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );
        if ( slideWithAudioMenu.isNewAndEmpty() ) {
                slideWithAudioMenu.setEscapeAction( new CreateOptionsMenu() );

                slideWithAudioMenu.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                // efectos sonoros
                this->labelEffects = new Label( langStringEffects->getText() );

                // música
                this->labelMusic = new Label( langStringMusic->getText() );

                // play the melody of scenery on entry to a room or don’t
                this->playRoomTunes = new Label( langStringRoomTunes->getText() );

                // create the menu

                this->audioOptions = new MenuWithValues( );

                audioOptions->addOption( labelEffects );
                ss.str( std::string() ); // clear ss
                ss << SoundManager::getInstance().getVolumeOfEffects();
                audioOptions->setValueOf( labelEffects, ss.str() );

                audioOptions->addOption( labelMusic );
                ss.str( std::string() );
                ss << SoundManager::getInstance().getVolumeOfMusic();
                audioOptions->setValueOf( labelMusic, ss.str() );

                audioOptions->addOption( playRoomTunes );
                audioOptions->setValueOf( playRoomTunes, GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );

                audioOptions->setVerticalOffset( 35 );

                // add this menu to the screen

                slideWithAudioMenu.addWidget( audioOptions );
        }
        else {
                // update labels
                audioOptions->setValueOf( playRoomTunes, GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );
        }

        if ( slideWithAudioMenu.getKeyHandler() == nilPointer )
                slideWithAudioMenu.setKeyHandler( this->audioOptions );

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );

        allegro::emptyKeyboardBuffer();

        while ( true )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        // get the key typed by the user
                        std::string theKey = allegro::nextKey() ;

                        if ( theKey == "Escape" )
                        {
                                allegro::emptyKeyboardBuffer();
                                slideWithAudioMenu.handleKey ( theKey );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( audioOptions->getActiveOption () == labelMusic || audioOptions->getActiveOption () == labelEffects )
                                {
                                        int musicVolume = SoundManager::getInstance().getVolumeOfMusic();
                                        int effectsVolume = SoundManager::getInstance().getVolumeOfEffects();
                                        int value = ( audioOptions->getActiveOption () == labelMusic ) ? musicVolume : effectsVolume ;
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

                                                if ( audioOptions->getActiveOption () == labelMusic )
                                                {
                                                        audioOptions->setValueOf( labelMusic, ss.str() );
                                                        SoundManager::getInstance().setVolumeOfMusic( value );
                                                }
                                                else if ( audioOptions->getActiveOption () == labelEffects )
                                                {
                                                        audioOptions->setValueOf( labelEffects, ss.str() );
                                                        SoundManager::getInstance().setVolumeOfEffects( value );
                                                        SoundManager::getInstance().stopEverySound ();
                                                        SoundManager::getInstance().play ( "menus", "effect", /* loop */ false );
                                                }
                                        }
                                }
                                else if ( audioOptions->getActiveOption () == playRoomTunes )
                                {
                                        if ( theKey == "Left" || theKey == "Right" || theKey == "o" || theKey == "p" )
                                        {
                                                GameManager::getInstance().togglePlayMelodyOfScenery ();
                                                audioOptions->setValueOf( playRoomTunes, GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );

                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                        slideWithAudioMenu.getKeyHandler()->handleKey( theKey );

                                allegro::emptyKeyboardBuffer();
                                audioOptions->redraw ();
                        }
                }

                // no te comas la CPU
                // do not eat the CPU
                somn::milliSleep( 25 );
        }
}
