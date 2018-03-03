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

#include <list>
#include <string>
#include <ext/hash_map>
#include <iostream>
#include <cassert>
#include <allegro.h>
#include "alogg/aloggpth.h"
#include "alogg/aloggint.h"
#include "csxml/SoundsXML.hpp"
#include "Ism.hpp"


/**
 * Definición de la función hash para poder emplear std::string en plantillas __gnu_cxx::hash_map
 * Código de Stefan Olsson
 */
namespace __gnu_cxx
{

template< > struct hash< std::string >
{
        size_t operator()( const std::string& x ) const
        {
                return hash< const char* >() ( x.c_str() );
        }
};

}


namespace isomot
{

class SampleData ;
class SoundData ;


class SoundManager
{

private:

        SoundManager( ) ;

public:

        ~SoundManager( ) ;

        static SoundManager* getInstance () ;

        /**
         * Crea la lista de sonidos a partir de la información extraída por el gestor XML del archivo
         * que almacena los sonidos
         * @param fileName Nombre del archivo XML que contiene los datos de los sonidos
         */
        void readListOfSounds ( const std::string& fileName ) ;

        /**
         * Reproduce un sonido
         * @param label Item that gives out sound
         * @param activity Activity of item
         * @param loop Si vale true, el sonido se reproduce continuamente
         */
        void play ( const std::string& label, const ActivityOfItem& activity, bool loop = false ) ;

        /**
         * Detiene la reproducción de un sonido
         * @param label Item that gives out sound
         * @param activity Activity of item
         */
        void stop ( const std::string& label, const ActivityOfItem& activity ) ;

        /**
         * Detiene la reproducción de todos los sonidos
         */
        void stopEverySound () ;

        /**
         * Reproduce un archivo Ogg
         * @param fileName Nombre del archivo
         */
        void playOgg ( const std::string& fileName, bool loop ) ;

        /**
         * Detiene la reproducción en curso de un archivo Ogg
         */
        void stopAnyOgg () ;

        /**
         * Indica si un determinado archivo Ogg está reproduciéndose
         * @param fileName Nombre del archivo Ogg
         * @return true si está sonando o false en caso contrario
         */
        bool isPlayingOgg ( const std::string& fileName ) {  return this->oggPlaying == fileName && oggPlayer != nilPointer && oggPlayer->alive != 0 ;  }

private:

        /**
         * Busca un sonido en la tabla
         * @param label Item that gives out sound
         * @param activity Activity of item
         * @return El sonido ó 0 si no se encontró
         */
        SampleData* findSample ( const std::string& label, const ActivityOfItem& activity ) ;

        /**
         * Traduce el identificador de un estado empleado por Isomot a una cadena
         * de caracteres utilizada en la tabla de sonidos
         * @return Una cadena de caracteres con alguno de los estado manejados por la tabla de sonidos
         */
        std::string translateActivityOfItemToString ( const ActivityOfItem& activity ) ;

private:

        static SoundManager* instance ;

        /**
         * Nombre del archivo XML que contiene los datos de los sonidos
         */
        std::string fileName ;

        /**
         * Subproceso encargado de la reproducción de archivos Ogg
         */
        alogg_thread* oggPlayer ;

        /**
         * Segmento del archivo Ogg que se está reproduciendo
         */
        alogg_stream* oggStream ;

        /**
         * Nombre del tema que se está reproduciendo
         */
        std::string oggPlaying ;

        /**
         * Volumen de los efectos sonoros
         */
        unsigned int effectsVolume ;

        /**
         * Volumen de la música
         */
        unsigned int musicVolume ;

        /**
         * Tabla de sonidos
         */
        std::list < SoundData > soundData ;

public:

        /**
         * Establece el volumen de los efectos sonoros
         * @param volume Un valor comprendido entre 0 y 99
         */
        void setVolumeOfEffects ( unsigned int volume ) {  this->effectsVolume = volume ;  }

        /**
         * Volumen de los efectos sonoros
         * @return Un valor comprendido entre 0 y 99
         */
        unsigned int getVolumeOfEffects () const {  return this->effectsVolume ;  }

        /**
         * Establece el volumen de la música
         * @param volume Un valor comprendido entre 0 y 99
         */
        void setVolumeOfMusic ( unsigned int volume ) ;

        /**
         * Volumen de la música
         * @return Un valor comprendido entre 0 y 99
         */
        unsigned int getVolumeOfMusic () const {  return this->musicVolume ;  }

};


/**
 * Associates sound sample with reproduction number
 */

class SampleData
{

public:

        /**
         * Digital sound sample
         */
        SAMPLE * sample ;

        /**
         * Voice number for sound reproduction
         */
        int voice ;

};


/**
 * Data of sounds
 */

class SoundData
{

        friend class SoundManager ;

public:

        /**
         * Constructor
         * @param label Unique label of this sound
         */
        SoundData( const std::string& label ) ;

        void addSound ( const std::string& activity, const std::string& sampleFileName ) ;

        /**
         * Look for sound
         * @param event When to play that sound
         */
        SampleData* find ( const std::string& event ) ;

private:

        /**
         * Unique label of this sound
         */
        std::string label ;

        /**
         * Hash table of activity / sound pairs. Activity is string from XML file of sounds
         */
        __gnu_cxx::hash_map< std::string, SampleData > table ;

public:

        std::string getLabel () const {  return this->label ;  }

};


struct EqualSoundData : public std::binary_function < SoundData, std::string, bool >
{
        bool operator()( const SoundData& soundData, const std::string& label ) const ;
};

}

#endif
