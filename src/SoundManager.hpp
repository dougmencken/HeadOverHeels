// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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


namespace isomot
{

class SoundManager
{

private:

        SoundManager( ) ;

public:

        ~SoundManager( ) ;

        static SoundManager* getInstance () ;

        /**
         * Crea la lista de sonidos a partir de la información extraída por el gestor XML del archivo que almacena los sonidos
         * @param xmlFile Name of XML file with info about game’s sounds
         */
        void readSounds ( const std::string& xmlFile ) ;

        void addSound ( const std::string& label, const std::string& activity, const std::string& sampleFile ) ;

        /**
         * Reproduce un sonido
         * @param label Item that gives out sound
         * @param activity Activity of item
         * @param loop Si vale true, el sonido se reproduce continuamente
         */
        void play ( const std::string& label, const ActivityOfItem& activity, bool loop = false ) ;

        /**
         * Detiene la reproducción de un sonido
         */
        void stop ( const std::string& label, const ActivityOfItem& activity ) ;

        /**
         * Detiene la reproducción de todos los sonidos
         */
        void stopEverySound () ;

        /**
         * Reproduce un archivo Ogg
         */
        void playOgg ( const std::string& oggFile, bool loop ) ;

        void stopOgg () {  oggPlayingThread.stop() ;  }

        allegro::Sample * getSampleFor ( const std::string& label, const std::string& event ) ;

        allegro::Sample * getSampleFor ( const std::string& label, const ActivityOfItem& activity )
        {
                return getSampleFor( label, translateActivityToString( activity ) ) ;
        }

        void setVolumeOfEffects ( unsigned int volume ) {  this->effectsVolume = ( volume <= 99 ) ? volume : 99 ;  }

        unsigned int getVolumeOfEffects () const {  return this->effectsVolume ;  }

        void setVolumeOfMusic ( unsigned int volume ) ;

        unsigned int getVolumeOfMusic () const {  return this->musicVolume ;  }

protected:

        std::string translateActivityToString ( const ActivityOfItem& activity ) ;

private:

        static SoundManager* instance ;

        /**
         * Subprocess for playing Ogg music
         */
        allegro::ogg::ThreadPlaysStream oggPlayingThread ;

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

        std::map < ActivityOfItem, std::string > activityToString ;

};

}

#endif
