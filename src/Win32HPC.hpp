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

#ifndef WIN32HPC_HPP_
#define WIN32HPC_HPP_

#include <allegro.h>
#include <winalleg.h>

/**
 * Cronómetro de alta precisión para sistemas Windows
 */
class HPC
{
public:

        HPC();

        /**
         * Pone en marcha el cronómetro
         * @return true si el sistema dispone de un cronómetro de alta precisión
         */
        bool start();

        /**
         * Devuelve el número de segundos transcurridos desde que el cronómetro se puso en marcha
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
         * @preconditions El cronómetro debe estar detenido
         */
        void restart();

protected:

        /**
         * Frecuencia
         */
        LARGE_INTEGER frequency;

        /**
         * Instante en el que se puso en marcha el cronómetro
         */
        LARGE_INTEGER startTime;

        /**
         * Instante en el que se detiene el cronómetro
         */
        LARGE_INTEGER stopTime;

        /**
         * Periodo de tiempo transcurrido entre que el cronómetro se paró y se volvió a poner en marcha
         */
        LARGE_INTEGER period;
};

#endif //WIN32HPC_HPP_
