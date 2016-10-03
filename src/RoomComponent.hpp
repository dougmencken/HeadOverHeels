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

#ifndef ROOMCOMPONENT_HPP_
#define ROOMCOMPONENT_HPP_

#include <allegro.h>

namespace isomot
{

// Declaraciones adelantadas
class Mediator;

/**
 * Interfaz para cualquier elemento que forma parte de UNA sala
 * */
class RoomComponent
{
public:

	RoomComponent();

	virtual ~RoomComponent() {}

	/**
	 * Dibuja el componente
	 * @param destination Imagen donde se realizará el dibujo
	 */
	virtual void draw(BITMAP* destination) = 0;

	/**
	 * Añade un nuevo componente a la sala
	 * @param component El nuevo componente
	 */
	virtual void addComponent(RoomComponent* component) {}

	/**
	 * Elimina un componente de la sala
	 * @param component El componente a eliminar
	 */
	virtual void removeComponent(RoomComponent* component) {}

protected:

  /**
   * Mediador entre los elementos pertenecientes a una sala
   */
  Mediator* mediator;

public: // Operaciones de consulta y actualización

  /**
   * Establece el mediador entre los elementos pertenecientes a la sala
   * @param mediator El mediador de la sala
   */
  void setMediator(Mediator* mediator);

  /**
   * Mediador entre los elementos pertenecientes a la sala
   * @return mediator El mediador de la sala
   */
  Mediator* getMediator();
};

}

#endif //ROOMCOMPONENT_HPP_
