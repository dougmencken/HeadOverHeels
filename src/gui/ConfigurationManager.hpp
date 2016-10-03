// Head over Heels (A remake by helmántika.org)
//
// © Copyright 2008 Jorge Rodríguez Santos <jorge@helmantika.org>
// © Copyright 1987 Ocean Software Ltd. (Original game)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef CONFIGURATIONMANAGER_HPP_
#define CONFIGURATIONMANAGER_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <ext/hash_map>
#include <allegro.h>
#include "csxml/ConfigurationXML.hpp"
#include "Ism.hpp"

namespace gui
{

/**
 * Almacena la configuración del juego que puede cambiar el usuario:
 * el idioma, las teclas y el nivel de volumen de los efectos sonoros
 * y la música
 */
class ConfigurationManager
{
public:

  /**
   * Constructor
   * @param fileName Archivo XML con la configuración del juego
   */
  ConfigurationManager(const std::string& fileName);

  /**
   * Lee los datos de configuración y los almacena en los atributos para que
   * posteriormente puedan leerlos otros objetos de la interfaz de usuario
   */
  void read();

  /**
   * Guarda los datos de configuración almacenados en los atributos que han
   * podido modificar otros objetos de la interfaz de usuario
   */
  void write();

private:

  /**
   * Archivo XML con la configuración del juego
   */
  std::string fileName;

  /**
   * Idioma empleado en la interfaz de usuario
   */
  std::string language;

  /**
   * Tabla con las teclas empleadas en el juego
   */
  __gnu_cxx::hash_map<int, int> keys;

  /**
   * Volumen de los efectos de sonido; rango 0-99
   */
  int soundFxVolume;

  /**
   * Volumen de la música; rango 0-99
   */
  int musicVolume;

public: // Operaciones de consulta y actualización

  /**
   * Asigna el idioma empleado en la interfaz de usuario
   * @param language una cadena de caracteres con el formato LLL_CC donde LLL es un código de
   * idioma según la norma ISO 639-2 y CC es un código de país según la norma ISO 3166
   */
  void setLanguage(const std::string& language) { this->language = language; }

  /**
   * Idioma empleado en la interfaz de usuario
   * @return Una cadena de caracteres con el formato LLL_CC donde LLL es un código de
   * idioma según la norma ISO 639 y CC es un código de país según la norma ISO 3166
   */
  std::string getLanguage() { return this->language; }

  /**
   * Asigna una tecla a una acción del juego
   * @param gameKey Una acción del juego
   * @param key Una tecla según la codificación de Allegro: KEY_LEFT, KEY_A, etc
   */
  void setKey(const isomot::GameKey& gameKey, int keyCode) { this->keys[static_cast<int>(gameKey)] = keyCode; }

  /**
   * Tecla ligada a una acción del juego
   * @param gameKey Una acción del juego
   * @return Una tecla según la codificación de Allegro: KEY_LEFT, KEY_A, etc
   */
  int getKey(const isomot::GameKey& gameKey) { return this->keys[static_cast<int>(gameKey)]; }

  /**
   * Asigna el volumen de los efectos de sonido
   * @param volume Un valor entre 0 y 99, ambos inclusive
   */
  void setSoundFxVolume(int volume) { this->soundFxVolume = volume; }

  /**
   * Volumen de los efectos de sonido
   * @return Un valor entre 0 y 99, ambos inclusive
   */
  int getSoundFxVolume() const { return this->soundFxVolume; }

  /**
   * Asigna el volumen de la música
   * @param volume Un valor entre 0 y 99, ambos inclusive
   */
  void setMusicVolume(int volume) { this->musicVolume = volume; }

  /**
   * Volumen de la música
   * @return Un valor entre 0 y 99, ambos inclusive
   */
  int getMusicVolume() { return this->musicVolume; }
};

}

#endif /*CONFIGURATIONMANAGER_HPP_*/
