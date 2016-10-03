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

#ifndef GUIMANAGER_HPP_
#define GUIMANAGER_HPP_

#include <string>
#include <map>
#include <cassert>
#include <allegro.h>
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
   * Constructor. Carga y almacena el conjunto de imágenes empleadas por la interfaz de
   * usuario, así como manda crear todos las fuentes tipográficas utilizadas
   */
  GuiManager();

public:

	~GuiManager();

  /**
   * Único objeto de esta clase para toda la aplicación
   * @return Un puntero al objeto único
   */
  static GuiManager* getInstance();

  /**
   * Envía la orden para crea el menú principal y empieza a mostrarlo en pantalla
   */
  void start();

  /**
   * Muestra la pantalla actual
   */
  void changeScreen(Screen* screen);

  /**
   * Actualiza el contenido de la pantalla
   */
  void refresh();

	/**
	 * Busca una imagen usada en la interfaz de usuario
	 * @param name Identificador textual asignado a la imagen
	 * @return La imagen ó 0 si no existe
	 */
	BITMAP* findImage(const std::string& name);

	/**
	 * Establece el idioma de la interfaz de usuario
	 * @param language Código ISO de la lengua
	 */
	void assignLanguage(const std::string& language);

	/**
	 * Suspende la presentación de la interfaz de usuario. Conduce al cierre de la aplicación
	 */
	void suspend() { this->active = false; }

protected:

  /**
   * Inicializa Allegro, establece el modo de vídeo e instala el controlador del teclado
   */
  void allegroSetup();

private:

  /**
   * Único objeto de esta clase para toda la aplicación
   */
  static GuiManager* instance;

  /**
   * Pantalla mostrada por la interfaz
   */
  Screen* screen;

  /**
   * Imagen donde se dibujará la interfaz gráfica
   */
  BITMAP* destination;

  /**
   * El gestor de la configuración del juego
   */
  ConfigurationManager* configurationManager;

  /**
   * El gestor de idioma
   */
  LanguageManager* languageManager;

  /**
   * Imágenes empleadas en la interfaz de usuario. Cada imagen tiene asociado un
   * identificador textual
   */
  std::map<std::string, BITMAP*> images;

  /**
   * Indica que la presentación de la interfaz y la gestión del teclado está activa
   */
  bool active;

public: // Operaciones de consulta y actualización

  /**
   * Devuelve el gestor de la configuración del juego
   * @return Un puntero al gestor ó 0 si no está creado
   */
  ConfigurationManager* getConfigurationManager() const;

  /**
   * Devuelve el gestor de idioma
   * @return Un puntero al gestor ó 0 si no está creado
   */
  LanguageManager* getLanguageManager() const;
};

}

#endif /*GUIMANAGER_HPP_*/
