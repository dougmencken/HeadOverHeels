// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Font_hpp_
#define Font_hpp_

#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <allegro.h>
#include "Gui.hpp"

namespace gui
{

const unsigned short CharactersPerRow = 16;
const unsigned short RowsPerBitmapFont = 21;

/**
 * El juego utiliza tipos de letras basados en imágenes. La correspondencia entre los
 * caracteres almacenados en la imagen y su código octal UTF-8 se almacenan en la variable
 * gui::characters. Los caracteres pueden representarse de un determinado color y al doble
 * de su tamaño original
 */
class Font
{
public:

  /**
   * Constructor
   * @param fontName Nombre de la fuente tipográfica empleado como identificador
   * @param fontFile Nombre del archivo de imagen que contienen la fuente tipográfica
   * @param color Color de los caracteres. El color se puede cambiar sólo si los caracteres son blancos
   * @param doubleSize Si vale true se doblará la altura de los caracteres
   */
  Font(const std::string& fontName, const std::string& fontFile, int color = makecol(255, 255, 255), bool doubleSize = false);

  virtual ~Font();

  /**
   * Devuelve el carácter especificado
   * @param Un carácter UTF-8
   * @return La imagen del carácter o la imagen del símbolo '?' si no se encontró
   */
  BITMAP* getChar(const std::string& character);

private:

  /**
   * Nombre de la fuente tipográfica empleado como identificador
   */
  std::string fontName;

  /**
   * Anchura de cada carácter en píxeles. Las fuentes caligráficas son monoespaciadas
   */
  unsigned short charWidth;

  /**
   * Altura de cada carácter en píxeles. Las fuentes caligráficas son monoespaciadas
   */
  unsigned short charHeight;

  /**
   * Conjunto de caracteres disponibles
   */
  std::vector<BITMAP*> charVector;

public: // Operaciones de consulta y actualización

  /**
   * Nombre de la fuente tipográfica empleado como identificador
   * @return Una cadena con el nombre de la fuente
   */
  std::string getFontName() const { return fontName; }

  /**
   * Anchura de cada carácter en píxeles. Las fuentes caligráficas son monoespaciadas
   * @return Un número mayor que cero
   */
  unsigned short getCharWidth() const  { return charWidth; }

  /**
   * Altura de cada carácter en píxeles. Las fuentes caligráficas son monoespaciadas
   */
  unsigned short getCharHeight() const { return charHeight; }
};

}

#endif
