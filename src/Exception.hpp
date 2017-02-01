// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Exception_hpp_
#define Exception_hpp_

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

#endif
