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

#ifndef LANGUAGEMANAGER_HPP_
#define LANGUAGEMANAGER_HPP_

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>
#include "csxml/LanguageXML.hpp"
#include "Gui.hpp"

namespace gui
{

// Declaraciones adelantadas
class LanguageText;

/**
 * Lee las cadenas de texto usadas en el juego de un archivo XML y las almacena
 * en una estructura de datos a la que acceden otros objetos de la interfaz de usuario
 */
class LanguageManager
{
public:

  /**
   * Constructor
   * @param fileName Archivo XML con los textos empleados en el juego
   */
	LanguageManager(const std::string& fileName);

	~LanguageManager();

	/**
	 * Busca una cadena de idioma
	 * @param id Cadena identificativa
	 * @return La cadena si se encontró ó 0 en caso contrario
	 */
	LanguageText* findLanguageString(const std::string& id);

private:

  /**
   * Analiza el archivo XML y extrae las cadenas ligándolas a un identificador textual
   * @param fileName Archivo XML con los textos empleados en el juego
   */
  void parse(const std::string& fileName);

private:

  /**
   * Textos empleados en la interfaz de usuario. Cada elemento se compone de un identificador
   * único, su posición en la pantalla y la cadena con el texto
   */
  std::list<LanguageText*> texts;

};

struct LanguageLine
{
  std::string text;

  std::string font;

  std::string color;
};

class LanguageText
{
public:

  LanguageText(const std::string& id, unsigned int x, unsigned int y);

private:

  std::string id;

  unsigned int x;

  unsigned int y;

  std::vector<LanguageLine> lines;

public:

  void addLine(const std::string& text);

  void addLine(const std::string& text, const std::string& font, const std::string& color);

  std::string getId() const { return this->id; }

  unsigned int getX() const { return this->x; }

  unsigned int getY() const { return this->y; }

  std::string getFirstLineText() const { return this->lines[0].text; }

  std::string getFirstLineFont() const { return this->lines[0].font; }

  std::string getFirstLineColor() const { return this->lines[0].color; }

  std::string getText() const { return this->getFirstLineText(); }

  std::string getFont() const { return this->getFirstLineFont(); }

  std::string getColor() const { return this->getFirstLineColor(); }

  LanguageLine* getFirstLine() { return &(this->lines[0]); }

  LanguageLine* getLine(size_t number) { return &(this->lines[number]); }

  size_t getLinesCount() { return this->lines.size(); }
};

/**
 * Objeto-función usado como predicado en la búsqueda de una cadena de idioma
 */
struct EqualLanguageString : public std::binary_function<LanguageText*, std::string, bool>
{
  bool operator()(const LanguageText* lang, const std::string& id) const;
};

}

#endif /*LANGUAGEMANAGER_HPP_*/
