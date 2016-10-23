// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Wall_hpp_
#define Wall_hpp_

#include <utility>
#include <allegro.h>
#include "Ism.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"

namespace isomot
{

/**
 * Un segmento de una pared de la sala
 */
class Wall : public Drawable, public Mediated
{
public:

	/**
	 * Constructor
	 * @param axis Eje de la pared: AXIS_X para la pared este y AXIS_Y para la pared norte
	 * @param index Posición que ocupa el segmento en la pared; cuanto menor sea el número
	 *              más cerca estará del origen de coordenadas
	 * @param image Imagen del segmento de la pared
	 */
	Wall( Axis axis, int index, BITMAP* image );

	virtual ~Wall();

	/**
	 * Calcula el desplazamiento de la imagen de la pared en función de los atributos actuales
	 */
	void calculateOffset();

	/**
	 * Dibuja el segmento de la pared
	 */
	void draw( BITMAP* where );

private:

	/**
	 * Eje de la pared: AXIS_X para la pared este y AXIS_Y para la pared norte
	 */
	Axis axis;

	/**
	 * Posición que ocupa el segmento en la pared; cuanto menor sea el número más cerca estará del
	 * origen de coordenadas
	 */
	int index;

	/**
	 * Desplazamiento de la imagen del muro en los ejes X e Y, respectivamente
	 */
	std::pair<int, int> offset;

	/**
	 * Gráfico del segmento de una pared
	 */
	BITMAP* image;

public: // Operaciones de consulta y actualización

	/**
	 * Eje donde está situada la pared
	 * @return AXIS_X, la pared este; y, AXIS_Y, la pared norte
	 */
	Axis getAxis() { return axis; }

	/**
	 * Posición que ocupa el segmento en la pared; cuanto menor sea el número más cerca estará del
	 * origen de coordenadas
	 * @return Un número positivo
	 */
	int getIndex() { return index; }
};

}

#endif
