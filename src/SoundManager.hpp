// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
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
#include <algorithm>
#include <iostream>
#include <functional>
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

class SampleData;
class SoundData;

/**
 * Gestor de los efectos de sonido. Almacena todos los sonidos emitidos por los
 * diferentes elementos del juego en una tabla cuyo campo principal es la etiqueta
 * que identifica al elemento. Cada elemento tiene un sonido determinado asociado
 * a una acción que normalmente coincide con alguno de sus estados
 */
class SoundManager
{

private:

        SoundManager( ) ;

public:

        ~SoundManager( ) ;

        /**
         * Único objeto de esta clase para toda la aplicación
         * @return Un puntero al objeto único
         */
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
        bool isPlayingOgg ( const std::string& fileName ) {  return this->oggPlaying.compare( fileName ) == 0 && oggPlayer && oggPlayer->alive != 0 ;  }

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

        /**
         * Único objeto de esta clase para toda la aplicación
         */
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
 * Estructura que asocia cada muestra de sonido a un indicador de reproducción
 */
class SampleData
{

public:

        /**
         * Sonido digital manejado por Allegro
         */
        SAMPLE* sample;

        /**
         * Número de voz asignada a la reproducción del sonido
         */
        int voice;

};

/**
 * Almacena los sonidos del juego. Cada sonido está ligado a un par elemento/estado
 */
class SoundData
{

public:

        /**
         * Constructor
         * @param label Unique label of item
         */
        SoundData( const std::string& label ) ;

        /**
         * Liga un sonido a un elemento del juego
         * @param state Descripción del estado
         * @param sampleFileName Nombre del archivo de sonido
         */
        void add ( const std::string& state, const std::string& sampleFileName ) ;

        /**
         * Busca un sonido
         * @param state Estado asociado al sonido
         * @return Sonido digital manejado por Allegro
         */
        SampleData* find ( const std::string& state ) ;

public:

        /**
         * Unique label of item
         */
        std::string label ;

        /**
         * Hash table of activity / sound pairs. Activity is string from XML file of sounds
         */
        __gnu_cxx::hash_map< std::string, SampleData > table ;

        /**
         * Ruta al archivo ejecutable. Se necesita para cargar los sonidos
         */
        std::string path ;

public:

        std::string getLabel () const {  return this->label ;  }

};


struct EqualSoundData : public std::binary_function < SoundData, std::string, bool >
{
        bool operator()( const SoundData& soundData, const std::string& label ) const ;
};

}

#endif
