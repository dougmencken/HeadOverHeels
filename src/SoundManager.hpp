// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef SoundManager_hpp_
#define SoundManager_hpp_

#include <string>
#include <list>
#include <map>
#include <iostream>

#include "WrappersAllegro.hpp"

#include "ActivityOfItem.hpp"


namespace iso
{

class SoundManager
{

private:

        SoundManager( const std::string & audioInterface = "auto" ) ;

public:

        ~SoundManager () ;

        static void setAudioInterface( const std::string & interface ) {  audioInterface = interface ;  }

        static SoundManager & getInstance () ;

        /**
         * @param xmlFile the name of XML file with the info about the game’s sounds
         */
        void readSounds ( const std::string& xmlFile ) ;

        void addSound ( const std::string& label, const std::string& activity, const std::string& sampleFile ) ;

        bool isAudioInitialized () const {  return audioInitialized ;  }

        /**
         * Play a sound
         * @param label Item that gives out sound
         * @param activity Activity of item
         * @param loop If true, the playing is repeated
         */
        void play ( const std::string& label, const ActivityOfItem& activity, bool loop = false ) ;

        /**
         * Stops playing a sound
         */
        void stop ( const std::string& label, const ActivityOfItem& activity ) ;

        /**
         * Stops playing of all sounds
         */
        void stopEverySound () ;

        void playOgg ( const std::string& oggFile, bool loop ) ;

        void stopOgg () {  oggPlayer.stop() ;  }

        allegro::Sample * getSampleFor ( const std::string& label, const std::string& event ) ;

        allegro::Sample * getSampleFor ( const std::string& label, const ActivityOfItem& activity )
        {
                return getSampleFor( label, activityToString( activity ) ) ;
        }

        void setVolumeOfEffects ( unsigned int volume ) {  this->effectsVolume = ( volume <= 99 ) ? volume : 99 ;  }

        unsigned int getVolumeOfEffects () const {  return this->effectsVolume ;  }

        void setVolumeOfMusic ( unsigned int volume ) ;

        unsigned int getVolumeOfMusic () const {  return this->musicVolume ;  }

protected:

        std::string activityToString ( const ActivityOfItem& activity ) ;

private:

        static SoundManager * instance ;

        static std::string audioInterface ;

        bool audioInitialized ; // true after allegro::initAudio()

        /**
         * Subprocess for playing Ogg music
         */
        allegro::ogg::OggPlayer oggPlayer ;

        /**
         * Volumen de los efectos sonoros
         */
        unsigned int effectsVolume ;

        /**
         * Volumen de la música
         */
        unsigned int musicVolume ;

        /**
         * Map of item’s label to pairs of activity and name of file with sound
         */
        std::map < std::string /* label */, std::map< std::string /* event */, std::string /* file */ > > sounds ;

        /**
         * Maps name of file to sound sample
         */
        std::map < std::string /* file */, allegro::Sample * > samples ;

        std::map < ActivityOfItem, std::string > activityStrings ;

};

}

#endif
