// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GuiManager_hpp_
#define GuiManager_hpp_

#include <string>
#include <map>
#include <cassert>
#include <allegro.h>
#include <loadpng.h>
#include "Gui.hpp"

namespace gui
{

// Declaraciones adelantadas
class Screen;
class LanguageManager;
class ConfigurationManager;

/**
 * El gestor de la interfaz usuario almacena las imágenes empleadas por la interfaz, ordena
 * la creación de los tipos de letra usados e inicia el juego mostrando el menú principal
 */
class GuiManager
{

private:

       /**
        * Constructor
        * Carga y almacena el conjunto de imágenes empleadas por la interfaz de
        * usuario, así como manda crear todos las fuentes tipográficas utilizadas
        */
        GuiManager( ) ;

public:

        ~GuiManager( ) ;

       /**
        * Único objeto de esta clase para toda la aplicación
        * @return Un puntero al objeto único
        */
        static GuiManager* getInstance () ;

       /**
        * Envía la orden para crea el menú principal y empieza a mostrarlo en pantalla
        */
        void enter () ;

       /**
        * Muestra la pantalla actual
        */
        void changeScreen ( Screen* screen ) ;

       /**
        * Actualiza el contenido de la pantalla
        */
        void refresh () ;

       /**
        * Busca una imagen usada en la interfaz de usuario
        * @param name Identificador textual asignado a la imagen
        * @return La imagen ó 0 si no existe
        */
        BITMAP * findImage ( const std::string& name ) ;

       /**
        * Establece el idioma de la interfaz de usuario
        * @param language Código ISO de la lengua
        */
        void assignLanguage ( const std::string& language ) ;

       /**
        * Suspende la presentación de la interfaz de usuario. Conduce al cierre de la aplicación
        */
        void suspend () {  this->active = false ;  }

        bool isAtFullScreen () ;

       /**
        * Use it to toggle video at full screen & video in window
        */
        void toggleFullScreenVideo () ;

protected:

       /**
        * Inicializa Allegro, establece el modo de vídeo e instala el controlador del teclado
        */
        void allegroSetup () ;

private:

       /**
        * Único objeto de esta clase para toda la aplicación
        */
        static GuiManager * instance ;

       /**
        * Pantalla mostrada por la interfaz
        */
        Screen * screen ;

       /**
        * Imagen donde se dibujará la interfaz gráfica
        */
        BITMAP * picture ;

       /**
        * El gestor de la configuración del juego
        */
        ConfigurationManager * configurationManager ;

       /**
        * El gestor de idioma
        */
        LanguageManager * languageManager ;

       /**
        * Imágenes empleadas en la interfaz de usuario. Cada imagen tiene asociado un
        * identificador textual
        */
        std::map < std::string, BITMAP * > images ;

       /**
        * Indica que la presentación de la interfaz y la gestión del teclado está activa
        */
        bool active ;

       /**
        * Draw graphics at full screen when true or in window when false
        */
        bool atFullScreen ;

public: // Operaciones de consulta y actualización

       /**
        * Devuelve el gestor de la configuración del juego
        * @return Un puntero al gestor ó 0 si no está creado
        */
        ConfigurationManager* getConfigurationManager () const ;

       /**
        * Devuelve el gestor de idioma
        * @return Un puntero al gestor ó 0 si no está creado
        */
        LanguageManager* getLanguageManager () const ;

};

}

#endif
