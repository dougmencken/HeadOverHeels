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

#ifndef LINUXHPC_HPP_
#define LINUXHPC_HPP_

#include <sys/time.h>
#include <unistd.h>

/**
 * Cronómetro de alta precisión para sistemas Linux
 */
class HPC
{
public:

  HPC();

  virtual ~HPC();

  /**
   * Pone en marcha el cronómetro
   */
  void start();

  /**
   * Devuelve el tiempo transcurrido desde que el cronómetro se puso en marcha
   * @return Un valor en milisegundos
   */
  double getValue();

  /**
   * Pone a cero el cronómetro
   */
  void reset();

  /**
   * Detiene el cronómetro
   */
  void stop();

  /**
   * Reinicia el cronómetro
   */
  void restart();

protected:

  /**
   * Instante en el que se puso en marcha el cronómetro
   */
  timeval trestart;

  /**
   * Instante en el que se detiene el cronómetro
   */
  timeval tstop;

  /**
   * Periodo de tiempo transcurrido entre que el cronómetro se paró y se volvió a poner en
   * marcha
   */
  double period;

  /**
   * Huso horario
   */
  struct timezone tz;
};

#endif //LINUXHPC_HPP_
