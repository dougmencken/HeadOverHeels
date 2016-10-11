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

#ifndef ROOM_HPP_
#define ROOM_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include "Ism.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"

namespace isomot
{

// Declaraciones adelantadas
class FloorTile;
class Wall;
class Item;
class GridItem;
class FreeItem;
class PlayerItem;
class Door;
class Camera;
class ItemDataManager;
class TripleRoomStartPoint;

/**
 * Una sala dibujada en perspectiva isométrica. Se compone de una rejilla sobre la que se colocan la mayoría
 * de los elementos. Así mismo, pueden situarse losetas en el suelo y paredes en el perímetro de la sala
 */
class Room : public Drawable, public Mediated
{
public:

  /**
   * Constructor. Los parámetros especificados definen las dimensiones de la sala
   * @param identifier Identificador textual de la sala. Se hace coincidir con el nombre de su archivo
   * @param scenery Identificador textual del escenario al que pertenece la sala: jail, blacktooth,
   * marketplace, themoon, egyptus, penitentiary, safary o byblos
   * @param xTiles Número de losetas en el eje X
   * @param yTiles Número de losetas en el eje Y
   * @param tileSize Longitud del lado de una loseta en unidades isométricas
   * @param floorType Tipo de suelo
   */
        Room(const std::string& identifier, const std::string& scenery, int xTiles, int yTiles, int tileSize, const FloorId& floorType);

        virtual ~Room();

  /**
   * Añade un nueva loseta a la sala
   * @param floorTile La nueva loseta
   */
  void addItem( FloorTile* floorTile );

  /**
   * Añade un nuevo segmento de muro a la sala
   * @param wall El nuevo segmento de muro
   */
  void addWall( Wall* wall );

  /**
   * Añade una nueva puerta a la sala
   * @param door La nueva puerta
   */
  void addDoor( Door* door );

  /**
   * Añade un nuevo elemento rejilla de la sala
   * @param gridItem Un elemento rejilla
   */
  void addItem( GridItem* gridItem );

  /**
   * Añade un nuevo elemento libre de la sala
   * @param gridItem Un elemento libre
   */
  void addItem( FreeItem* freeItem );

  /**
   * Añade un nuevo jugador a la sala
   * @param gridItem Un elemento libre
   */
  void addPlayer( PlayerItem* playerItem );

  /**
   * Elimina una loseta de la sala
   * @param floorTile La loseta a eliminar
   */
  void removeItem( FloorTile* floorTile );

  /**
   * Elimina un elemento rejilla de la sala
   * @param gridItem Un elemento rejilla
   */
  void removeItem( GridItem* gridItem );

  /**
   * Elimina un elemento libre de la sala
   * @param gridItem Un elemento libre
   */
  void removeItem( FreeItem* freeItem );

  /**
   * Elimina un jugador de la sala
   * @param gridItem Un jugador
   */
  void removePlayer( PlayerItem* playerItem );

        /**
         * Dibuja la sala
         * @param destination Imagen donde se realizará el dibujo
         */
        void draw( BITMAP* destination = 0 );

        /**
         * Inicia la actualización de los elementos de la sala
         */
        void startUpdate();

        /**
         * Detiene la actualización de los elementos de la sala
         */
        void stopUpdate();

  /**
   * Calcula los límites reales de la sala en función de su tamaño y las puertas existentes
   */
  void calculateBounds();

  /**
   * Calcula las coordenadas de pantalla donde se sitúa el origen del espacio isométrico
   * @param hasNorthDoor La sala tiene una puerta al norte
   * @param hasNorthEast La sala tiene una puerta al este
   * @param deltaX Diferencia en el eje X a aplicar para ajustar la posición de la sala en pantalla
   * @param deltaY Diferencia en el eje Y a aplicar para ajustar la posición de la sala en pantalla
   */
  void calculateCoordinates(bool hasNorthDoor, bool hasEastDoor, int deltaX, int deltaY);

  /**
   * Establece el jugador que controla el usuario
   * @param playerId El identificador de alguno de los jugadores
   */
  void activatePlayer(const PlayerId& playerId);

  /**
   * Activa la sala. Implica la puesta en marcha de la actualización de los elementos y del dibujado
   */
  void activate();

  /**
   * Desactiva la sala. Implica la suspensión de la actualización de los elementos y del dibujado
   */
  void deactivate();

  /**
   * Cambia el jugador activo por el siguiente jugador presente en la sala
   * @return true si se ha podido hacer el cambio o false si el jugador activo es el único presente
   * en la sala. En este último caso, el cambio de jugador implica el cambio de la sala activa
   */
  bool changePlayer(ItemDataManager* itemDataManager);

  /**
   * Selecciona al jugador que queda vivo en la sala tras la pérdida de las vidas del otro jugador
   * @param itemDataManager Necesario para poder crear a un jugador simple a partir del compuesto
   * @return true si se ha cambiado el control al otro jugador o false si ya no hay más jugadores
   */
  bool alivePlayer(ItemDataManager* itemDataManager);

  /**
   * Calcula las coordenadas en las que se debe situar a un jugador para entrar a la sala
   * @param entry Entrada que toma el jugador
   * @param widthX Anchura del jugador en el eje X
   * @param widthY Anchura del jugador en el eje Y
   * @param northBound Límite norte de la sala. Coordenada X donde están los muros norte o la puerta norte
   * @param eastBound Límite este de la sala. Coordenada Y donde están los muros este o la puerta este
   * @param southBound Límite sur de la sala. Coordenada X donde están los muros sur o la puerta sur
   * @param westBound Límite oeste de la sala. Coordenada Y donde están los muros oeste o la puerta oeste
   * @param x Devuelve la coordenada X resultante
   * @param y Devuelve la coordenada Y resultante
   * @param z Devuelve la coordenada Z resultante
   * @return true si se pudieron calcular las coordenadas o false en contrario
   */
  bool calculateEntryCoordinates(const Direction& entry, int widthX, int widthY, int northBound, int eastBound, int southBound, int westBound, int* x, int* y, int* z);

  /**
   * Almacena una posición inicial de la cámara en una sala triple
   * @param direction Vía de entrada del jugador
   * @param x Diferencia que se debe aplicar a la coordenada de pantalla X para centrar la cámara
   * @param y Diferencia que se debe aplicar a la coordenada de pantalla Y para centrar la cámara
   */
  void addTripleRoomStartPoint(const Direction& direction, int x, int y);

  /**
   * Almacena los límites para el desplazamiento de la cámara en una sala triple
   */
  void assignTripleRoomBounds(int minX, int maxX, int minY, int maxY);

  /**
   * Busca una posición inicial de la cámara en una sala triple
   * @param direction Vía de entrada del jugador
   */
  TripleRoomStartPoint* findTripleRoomStartPoint(const Direction& direction);

private:

  friend class Mediator;

  /**
   * Identificador textual de la sala. Se hace coincidir con el nombre de su archivo
   */
  std::string identifier;

  /**
   * Identificador textual del escenario al que pertenece la sala: jail, blacktooth, marketplace,
   * themoon, egyptus, penitentiary, safary o byblos
   */
  std::string scenery;

  /**
   * Coordenadas de pantalla X e Y, respectivamente, donde está situada la coordenada origen de la sala
   */
  std::pair<int, int> coordinates;

  /**
   * Número de losetas de la sala en los ejes X e Y, respectivamente
   */
  std::pair<int, int> tilesNumber;

  /**
   * Longitud del lado de una loseta en unidades isométricas
   */
  int tileSize;

  /**
   * Tipo de suelo de la sala
   */
  FloorId floorType;

  /**
   * Indica si la sala está activa, es decir, si debe dibujarse
   */
  bool active;

  /**
   * Salida por la que algún jugador abandona la sala
   */
  Direction exit;

  /**
   * Índices que especifican el orden en el que se dibujarán las columnas de elementos rejilla que forman
   * la estructura de la sala. Se pintan en diagonales de derecha a izquierda
   */
  int* drawIndex;

  /**
   * Indica si se debe reiniciar la sala, es decir, si debe volver al estado existente tras su creación
   * El atributo normalmente se activa cuando un jugador pierde una vida
   */
  bool restart;

  /**
   * Grado de opacidad de las sombras desde 0, sin sombras, hasta 256, sombras totalmente opacas
   */
  short shadingScale;

  /**
   * Último identificador del motor asignado a un elemento rejilla
   */
  int lastGridId;

  /**
   * Último identificador del motor asignado a un elemento rejilla
   */
  int lastFreeId;

  /**
   * Coordenadas isométricas que delimitan la sala. Existe una por cada punto cardinal
   */
  unsigned short bounds[12];

  /**
   * El suelo de la sala formado por losetas
   */
  std::vector<FloorTile*> floor;

  /**
   * El muro este
   */
  std::vector<Wall*> wallX;

  /**
   * El muro norte
   */
  std::vector<Wall*> wallY;

  /**
   * Las puertas. Se almacenan es este orden: sur, oeste, norte, este y para las salas triples:
   * noreste, sureste, noroeste, suroeste, este-norte, este-sur, oeste-norte y oeste-sur
   */
  Door* doors[12];

  /**
   * Cámara encargada de centrar una sala grande en pantalla
   */
  Camera* camera;

  /**
   * Imagen donde se dibuja la sala activa
   */
  BITMAP* destination;

  /**
   * Posición inicial de la cámara en las salas triples según la vía de entrada
   */
  std::list<TripleRoomStartPoint> tripleRoomStartPoints;

  /**
   * Límites para mover la cámara a lo largo del eje X en una sala triple
   */
  std::pair<int, int> tripleRoomBoundX;

  /**
   * Límites para mover la cámara a lo largo del eje Y en una sala triple
   */
  std::pair<int, int> tripleRoomBoundY;

public: // Operaciones de consulta y actualización

  /**
   * Identificador textual de la sala. Se hace coincidir con el nombre de su archivo
   * @return Una cadena con el nombre del archivo y con la extensión ".xml" incluida
   */
  std::string getIdentifier() const { return identifier; }

  /**
   * Identificador textual del escenario al que pertenece la sala
   * @return jail, blacktooth, marketplace, themoon, egyptus, penitentiary, safary o byblos
   */
  std::string getScenery() const { return this->scenery; }

  /**
   * Coordenadas de pantalla X donde está situada la coordenada origen de la sala
   * @return Un número entero
   */
  int getX0() const { return coordinates.first; }

  /**
   * Coordenadas de pantalla Y donde está situada la coordenada origen de la sala
   * @return Un número entero
   */
  int getY0() const { return coordinates.second; }

  /**
   * Número de losetas de la sala en el eje X
   * @return Un número mayor o igual que dos
   */
  int getTilesX() const { return tilesNumber.first; }

  /**
   * Número de losetas de la sala en el eje Y
   * @return Un número mayor o igual que dos
   */
  int getTilesY() const { return tilesNumber.second; }

  /**
   * Longitud del lado de una loseta en unidades isométricas
   * @return Un número mayor que dos
   */
  int getTileSize() const { return tileSize; }

  /**
   * Tipo de suelo de la sala
   * @return El suelo puede se normal, inexistente o mortal
   */
  FloorId getFloorType() const { return floorType; }

  /**
   * Activa o desactiva la sala. Una sala desactivada no se dibuja
   * @return true para comenzar a dibujar la sala o false en caso contrario
   */
  void setActive(const bool active) { this->active = active; }

  /**
   * Indica si la sala está activa
   * @return true si se está dibujando la sala o false si la sala no está activa y no se está dibujando
   */
  bool isActive() const { return active; }

  /**
   * Límite de la sala
   * @param direction Un punto cardinal indicativo del límite que se quiere obtener
   * @return Un valor en unidades isométricas
   */
  unsigned short getBound(const Direction& direction) const { return bounds[direction]; }

  /**
   * Una puerta de la sala
   * @param direction Un punto cardinal que indica dónde está la puerta
   * @return Una puerta ó 0 si no hay puerta en el punto cardinal indicado
   */
  Door* getDoor(const Direction& direction) const;

  /**
   * Establece la salida por la que algún jugador abandona la sala
   * @param exit El suelo, el techo o la ubicación de alguna puerta
   */
  void setExit(const Direction& exit) { this->exit = exit; }

  /**
   * Salida por la que algún jugador abandona la sala
   * @return El suelo, el techo o la ubicación de alguna puerta
   */
  Direction getExit() const { return exit; }

  /**
   * Cámara encargada de centrar una sala grande en pantalla
   */
  Camera* getCamera() const;

  /**
   * Imagen donde se dibuja la sala activa
   * @return Un bitmap de Allegro
   */
  BITMAP* getDestination() { return destination; }

  /**
   * Límites para mover la cámara a lo largo del eje X en una sala triple
   */
  std::pair<int, int> getTripleRoomBoundX() const { return this->tripleRoomBoundX; }

  /**
   * Límites para mover la cámara a lo largo del eje Y en una sala triple
   */
  std::pair<int, int> getTripleRoomBoundY() const { return this->tripleRoomBoundY; }
};

/**
 * Registro para una terna de elementos compuesta por una dirección y un par de coordenadas
 * Se emplea para almacenar la posición inicial de la cámara en las salas triples en función
 * de la vía de entrada del jugador
 */
class TripleRoomStartPoint
{
public:

  TripleRoomStartPoint(const Direction& direction, int x, int y);

private:

  /**
   * Dirección de entrada a la sala
   */
  Direction direction;

  /**
   * Diferencia que se debe aplicar a la coordenada de pantalla X para centrar la cámara
   */
  int x;

  /**
   * Diferencia que se debe aplicar a la coordenada de pantalla X para centrar la cámara
   */
  int y;

public: // Operaciones de consulta y actualización

  /**
   * Dirección de entrada a la sala
   * @return Una dirección
   */
  Direction getDirection() const { return this->direction; }

  /**
   * Diferencia que se debe aplicar a la coordenada de pantalla X para centrar la cámara
   */
  int getX() const { return this->x; }

  /**
   * Diferencia que se debe aplicar a la coordenada de pantalla Y para centrar la cámara
   */
  int getY() const { return this->y; }
};

/**
 * Objeto-función usado como predicado en la búsqueda de una posición inicial de la cámara
 * en una sala triple
 */
class EqualTripleRoomStartPoint : public std::binary_function<TripleRoomStartPoint, Direction, bool>
{
public:
  bool operator()(TripleRoomStartPoint point, Direction direction) const;
};

}

#endif //ROOM_HPP_
