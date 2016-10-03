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

#ifndef ITEM_HPP_
#define ITEM_HPP_

#include <string>
#include <utility>
#include <allegro.h>
#include "Ism.hpp"
#include "HPC.hpp"
#include "RoomComponent.hpp"

namespace isomot
{

// Declaraciones adelantadas
class ItemData;
class Behavior;

/**
 * Interfaz que define los atributos y operaciones mínimas para poder crear un elemento
 * perteneciente a alguna sala
 */
class Item : public RoomComponent
{
public:

  /**
   * Constructor
   * @param itemData Datos invariables del elemento
   * @param z Posición espacial Z o a qué distancia está el elemento del suelo
   * @param direction Dirección inicial del elemento
   */
        Item(ItemData* itemData, int z, const Direction& direction);

  /**
   * Constructor copia. No copia los atributos que son punteros
   * @param item Un objeto de esta clase
   */
        Item(const Item& item);

        virtual ~Item();

  /**
   * Dibuja el componente
   * @param destination Imagen donde se realizará el dibujo
   */
        virtual void draw(BITMAP* destination);

  /**
   * Actualiza el comportamiento del elemento
   */
  virtual bool update();

  /**
   * Anima el elemento obteniendo el fotograma adecuado de la secuencia y cambiando su gráfico y su sombra
   * @return true si ha completado un ciclo de animación o false en caso contrario
   */
  bool animate();

  /**
   * Cambia todos los datos invariables del elemento. Se utiliza cuando hay una metamorfósis completa del
   * elemento como, por ejemplo, cuando un jugador se teletransporta que cambia por completo su aspecto
   * @param itemData Datos invariables del elemento
   */
  void changeItemData(ItemData* itemData, const Direction& direction);

  /**
   * Cambia la dirección del elemento y establece el fotograma adecuado
   * @param orientation La nueva dirección
   */
  void changeDirection(const Direction& direction);

  /**
   * Cambia el fotograma actual del elemento. El uso de esta operación es poco habitual ya que el
   * cambio de fotogramas normalmente viene dado por un cambio de dirección o por un avance en la
   * secuencia de animación. Sin embargo, hay casos donde es necesario su uso. Véase el comportamiento
   * del trampolín donde estando en reposo muestra un fotograma y plegado muestra otro; situaciones
   * que nada tienen que ver con direcciones y animaciones
   * @param frameIndex Índice del nuevo fotograma
   */
  void changeFrame(const unsigned int frameIndex);

  /**
   * Cambia la presentación gráfica del elemento, destruyendo la imagen procesada y señalando qué elementos
   * hay que volver a enmascarar
   * @param image Un fotograma del elemento
   */
  virtual void changeImage(BITMAP* image) = 0;

  /**
   * Cambia la sombra de la presentación gráfica del elemento, destruyendo la imagen procesada y señalando
   * qué elementos hay que volver a sombrear
   * @param image Una sombra de un fotograma del elemento
   */
  virtual void changeShadow(BITMAP* shadow) = 0;

  /**
   * Cambia el valor de la coordenada Z
   * @param value Nuevo valor para Z
   * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
   */
  virtual bool changeZ(int value);

  /**
   * Cambia el valor de la coordenada Z
   * @param value Valor que se sumará a la coordenada Z actual
   * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
   */
  virtual bool addZ(int value);

  /**
   * Comprueba si el elemento puede moverse a la posición indicada
   * @param x La nueva coordenada X
   * @param y La nueva coordenada Y
   * @param z La nueva coordenada Z
   * @param mode Modo para interpretar las coordenadas. Puede usarse Change para cambiarlas o Add para sumarlas
   * @return true si la posición es válida o false si hay colisión. En este último caso
   * la pila de colisiones almacenará los elementos que impiden el movimiento
   */
  virtual bool checkPosition(int x, int y, int z, const Mode& mode);

  /**
   * Crea el comportamiento del elemento según el identificador
   * @param behaviorId Identificador del comportamiento del elemento
   * @param extra Datos extra necesarios para configurar el comportamiento
   */
  void assignBehavior(const BehaviorId& id, void* extraData);

  /**
   * Establece el sentido de la animación del primer al último fotograma (hacia adelante)
   * Es el sentido establecido por defecto
   */
  void setForwardMotion();

  /**
   * Establece el sentido de la animación del último al primer fotograma (hacia atrás)
   */
  void setBackwardMotion();

protected:

  /**
   * Identificador único del elemento asignado por el motor
   */
  int id;

  /**
   * Etiqueta unívoca del elemento, es decir, no puede haber dos elementos de distinto tipo con la
   * misma etiqueta. Las etiquetas son constantes y no cadenas de caracteres
   */
  short label;

  /**
   * Posición espacial X en unidades isométricas
   */
  int x;

  /**
   * Posición espacial Y en unidades isométricas
   */
  int y;

  /**
   * Posición espacial Z en unidades isométricas
   */
  int z;

  /**
   * Datos invariables del elemento, aquellos que definen sus características básicas
   */
  ItemData* itemData;

  /**
   * Dirección actual del elemento
   */
  Direction direction;

  /**
   * Índice del vector que almacena todos los fotogramas. Señala el fotograma actual del elemento
   */
  unsigned int frameIndex;

  /**
   * Indica si la secuencia de animación se realiza marcha atrás
   */
  bool backwardMotion;

  /**
   * Estado del proceso de sombreado
   */
  ShadeStatus shadeStatus;

  /**
   * Fotograma actual del elemento sin procesar
   */
  BITMAP* image;

  /**
   * Sombra del fotograma actual del elemento
   */
  BITMAP* shadow;

  /**
   * Fotograma actual del elemento con las sombras de otros elementos proyectadas y, si procede,
   * enmascarado
   */
  BITMAP* processedImage;

protected:

  /**
   * Desplazamiento del fotograma procesado en los ejes X e Y, respectivamente, desde el píxel
   * que marca el punto origen de la sala (0,0,0)
   */
  std::pair<int, int> offset;

  /**
   * Cronómetro para controlar la velocidad de la animación
   */
  HPC motionTimer;

  /**
   * Comportamiento del elemento
   */
  Behavior* behavior;

  /**
   * Elemento que sirve de referencia para saber si éste debe ser movido cuando otros
   * estén debajo
   */
  Item* anchor;

public: // Operaciones de consulta y actualización

  /**
   * Establece el identificador único del elemento asignado por el motor
   * @param id Un número positivo
   */
  void setId(const int id) { this->id = id; }

  /**
   * Identificador único del elemento asignado por el motor
   * @return Un número positivo
   */
  int getId() const { return id; }

  /**
   * La etiqueta, el tipo de elemento
   * @return Un número positivo
   */
  short getLabel() const { return label; }

  /**
   * Posición espacial X en unidades isométricas
   * @return Un número positivo
   */
  int getX() const { return x; }

  /**
   * Posición espacial Y en unidades isométricas
   * @return Un número positivo
   */
  int getY() const { return y; }

  /**
   * Establece la posición espacial Z en unidades isométricas
   * @param z Un número positivo
   */
  void setZ(const int z) { this->z = z; }

  /**
   * Posición espacial Z en unidades isométricas
   * @return Un número positivo
   */
  int getZ() const { return z; }

  /**
   * Anchura del elemento en unidades isométricas en el eje X
   * @return Un número mayor que 2
   */
  int getWidthX() const;

  /**
   * Anchura del elemento en unidades isométricas en el eje Y
   * @return Un número mayor que 2
   */
  int getWidthY() const;

  /**
   * Altura del elemento en unidades isométricas (anchura en el eje Z)
   * @param height Un número mayor que 2
   */
  void setHeight(int height);

  /**
   * Altura del elemento en unidades isométricas (anchura en el eje Z)
   * @return Un número mayor que 2
   */
  int getHeight() const;

  /**
   * El elemento quitará una vida al jugador si éste lo toca
   * @return true si mata al jugador o false si es inofensivo
   */
  bool isMortal() const;

  /**
   * Cuántos fotogramas diferentes tiene el elemento para cada una de las direcciones posibles:
   * norte, sur, este y oeste
   * @return 1, 2 ó 4
   */
  unsigned char getDirectionFrames() const;

  /**
   * Tiempo en segundos necesario para que el elemento se mueva
   * @return Un número positivo
   */
  double getSpeed() const;

  /**
   * Tiempo en segundos necesario para que el elemento caiga
   * @return Un número real positivo
   */
  double getWeight() const;

  /**
   * Tiempo en segundos que será mostrado cada fotograma de la secuencia de animación
   * @return Un número real positivo
   */
  double getFramesDelay() const;

  /**
   * Número total de fotogramas de la secuencia de animación
   * @return Un número positivo
   */
  unsigned int getFramesNumber() const;

  /**
   * Establece la dirección actual del elemento. Se usa únicamente si el elemento
   * no tiene más de un fotograma, en caso contrario, se emplea changeDirection
   * @return Algún punto cardinal, arriba o abajo
   * @see Direction
   */
  void setDirection(const Direction& direction) { this->direction = direction; }

  /**
   * La dirección actual del elemento
   * @return Algún punto cardinal, arriba o abajo
   * @see Direction
   */
  Direction getDirection() { return direction; }

  /**
   * Fotograma actual del elemento sin procesar
   * @return Una imagen Allegro
   */
  BITMAP* getImage() { return image; }

  /**
   * Sombra del fotograma actual del elemento
   * @return Un imagen Allegro
   */
  BITMAP* getShadow() { return shadow; }

  /**
   * Establece el estado del proceso de sombreado
   * @param shadeStatus NO_SHADOW, sin sombra; MUST_BE_SHADY, debe ser sombreado; o, SHADY, sombreado
   */
  void setShadeStatus(const ShadeStatus& shadeStatus) { this->shadeStatus = shadeStatus; }

  /**
   * Establece el desplazamiento del fotograma procesado desde el píxel que marca el punto origen
   * de la sala (0,0,0)
   * @return Un par de valores, siendo el primero el desplazamiento en el eje X y el segundo en el eje Y
   */
  void setOffset(const std::pair<int, int>& offset) { this->offset = offset; }

  /**
   * Desplazamiento del fotograma procesado desde el píxel que marca el punto origen de la sala (0,0,0)
   * @return Un par de valores, siendo el primero el desplazamiento en el eje X y el segundo en el eje Y
   */
  std::pair<int, int> getOffset() const { return offset; }

  /**
   * Desplazamiento del fotograma procesado en el eje X, desde el píxel que marca el punto origen
   * de la sala (0,0,0)
   * @return Un número entero
   */
  int getOffsetX() const { return offset.first; }

  /**
   * Desplazamiento del fotograma procesado en el eje X, desde el píxel que marca el punto origen
   * de la sala (0,0,0)
   * @return Un número entero
   */
  int getOffsetY() const { return offset.second; }

  /**
   * Comportamiento del elemento
   * @return Un puntero a un comportamiento
   */
  Behavior* getBehavior();

  /**
   * Elemento que sirve de referencia para saber si éste debe ser movido cuando otros
   * estén debajo
   * @param Un elemento ó 0 si no hay ninguno
   */
  void setAnchor(Item* item);

  /**
   * Elemento que sirve de referencia para saber si éste debe ser movido cuando otros
   * estén debajo
   * @return Un elemento ó 0 si no hay ninguno
   */
  Item* getAnchor();
};

}

#endif //ITEM_HPP_
