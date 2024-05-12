// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef SoundManager_hpp_
#define SoundManager_hpp_

#include <string>
#include <map>

#include "WrappersAllegro.hpp"

#include "Activity.hpp"


class SoundManager
{

private:

        SoundManager( const std::string & audioInterface = "auto" ) ;

public:

        ~SoundManager () ;

        static void setAudioInterface( const std::string & interface ) {  audioInterface = interface ;  }

        static SoundManager & getInstance () ;

        /**
         * @param xmlFile the name of XML file with the info about sounds
         */
        void readSounds ( const std::string & xmlFile ) ;

        void addSound ( const std::string & item, const std::string & event, const std::string & sampleFile ) ;

        bool isAudioInitialized () const {  return audioInitialized ;  }

        /**
         * Plays the sound
         * @param item What emits the sound
         * @param event What event is the sound playing for
         * @param loop If true, the sound repeats in a loop
         */
        void play ( const std::string & item, const std::string & event, bool loop = false ) ;

        /**
         * Stops playing a sound
         */
        void stop ( const std::string & item, const std::string & event ) ;

        /**
         * Stops playing of all sounds
         */
        void stopEverySound () ;

        void playOgg ( const std::string& oggFile, bool loop ) ;

        void stopOgg () {  oggPlayer.stop() ;  }

        allegro::Sample * getSampleFor ( const std::string & item, const std::string & event ) ;

        void setVolumeOfEffects ( unsigned int volume ) {  this->effectsVolume = ( volume <= 99 ) ? volume : 99 ;  }

        unsigned int getVolumeOfEffects () const {  return this->effectsVolume ;  }

        void setVolumeOfMusic ( unsigned int volume ) ;

        unsigned int getVolumeOfMusic () const {  return this->musicVolume ;  }

        static std::string activityToNameOfSound ( const activities::Activity & activity ) ;

private:

        static SoundManager * instance ;

        static std::string audioInterface ;

        bool audioInitialized ; // true after allegro::initAudio()

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
         * Maps the kind of item to the map of activity to the name of the sound file 🤯
         */
        std::map < std::string /* item */, std::map< std::string /* event */, std::string /* file */ > > sounds ;

        /**
         * Maps the name of file to the audio sample
         */
        std::map < std::string /* file */, allegro::Sample * > samples ;

} ;

#endif
