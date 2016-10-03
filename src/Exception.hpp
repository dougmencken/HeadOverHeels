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

#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include <exception>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

namespace isomot
{

/**
 * Base para el manejo de excepciones en la aplicación
 */
class Exception : public std::exception
{
public:

  /**
   * Constructor
   * @param message Información sobre la excepción
   */
	explicit Exception(const std::string& message);

	virtual ~Exception() throw();

	/**
	 * Informa sobre la causa de la excepción
	 * @return Una cadena con el texto informativo
	 */
	virtual const char* what() const throw();

	/**
	 * Levanta la excepción
	 */
	virtual void raise() const;

	/**
	 * Copia el objeto
	 */
	virtual Exception* clone() const;

private:

  /**
   * Información sobre la excepción
   */
  std::string message;
};

/**
 * Excepción para el tratamiento de punteros nulos
 */
struct ENullPointer : public Exception
{
  explicit ENullPointer();
};

/**
 * Función plantilla para la comprobación de errores
 */
template<class E, class C> inline void Assert(C condition)
{
  if(!condition) throw E();
}

}

#endif //EXCEPTION_HPP_
