// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef UserControlled_hpp_
#define UserControlled_hpp_

#include <vector>
#include <utility>
#include "Behavior.hpp"
#include "HPC.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;
class ItemData;
class ItemDataManager;
class PlayerItem;

/**
 * Base para la creación de elementos controlador por el usuario. Contiene los atributos y acciones
 * que puede realizar un jugador y que se utilizan en la operación "update" que no es más que una
 * máquina de estados que junto con la operación "execute" modela el comportamiento del jugador
 */
class UserControlled : public Behavior
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador del comportamiento
   */
  UserControlled(Item* item, const BehaviorId& id);

	virtual ~UserControlled();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update() = 0;

  /**
   * Actualiza el estado del jugador en función de las órdenes dadas por el usuario
   */
  virtual void execute() = 0;

  /**
   * Asigna el identificador del estado actual del comportamiento y cambia el estado
   * del comportamiento según el identificador
   * @param Un identificador de estado
   * @param Elemento que emite el cambio de estado
   */
  virtual void changeStateId(const StateId& stateId, Item* sender = 0);

protected:

  /**
   * Pone en espera al jugador. Implica la presentación del primer fotograma de la
   * secuencia de animación según la orientación del jugador
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void wait(PlayerItem* playerItem);

  /**
   * Mueve al jugador a la velocidad establecida en los datos del elemento y en la
   * dirección de alguno de los cuatro puntos cardinales
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void move(PlayerItem* playerItem);

  /**
   * Mueve automáticamente al jugador a la velocidad establecida en los datos del elemento
   * y en la dirección de alguno de los cuatro puntos cardinales, una distancia igual al
   * número definido en los pasos automáticos (automaticSteps)
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void autoMove(PlayerItem* playerItem);

  /**
   * Desplaza al jugador a la velocidad del elemento que lo empuja y en la dirección de alguno
   * de los cuatro puntos cardinales y sus puntos intermedios, es decir, en ocho direcciones
   * posibles
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void displace(PlayerItem* playerItem);

  /**
   * Cancela el desplazamiento del jugador moviéndolo en la dirección contraria a la que está
   * siendo desplazado. Tiene el efecto de dejarlo parado en un punto al contrarrestar la fuerza
   * de desplazamiento. Este estado se utiliza normalmente cuando el jugador es arrastrado por
   * una cinta transportadora o algún elemento similar
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void cancelDisplace(PlayerItem* playerItem);

  /**
   * El jugador cae hasta topar con algo a la velocidad establecida en los datos del elemento
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void fall(PlayerItem* playerItem);

  /**
   * Hace saltar al jugador según la definición del salto realizada en las subclases
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void jump(PlayerItem* playerItem);

  /**
   * Hace planear al jugador a la velocidad establecida en los datos del elemento
   * y en la dirección de alguno de los cuatro puntos cardinales
   * @pre El jugador debe estar en el aire porque esté cayendo o haya saltado
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void glide(PlayerItem* playerItem);

  /**
   * Teletransporta al jugador desde otra sala
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void wayInTeletransport(PlayerItem* playerItem);

  /**
   * Teletransporta al jugador hacia otra sala
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void wayOutTeletransport(PlayerItem* playerItem);

  /**
   * Destruye al jugador por haber sido eliminado por un enemigo
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void destroy(PlayerItem* playerItem);

  /**
   * El jugador dispara algo que paralizará a los elementos mortales móviles
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void shot(PlayerItem* playerItem);

  /**
   * El jugador toma el elemento que haya debajo de él
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void take(PlayerItem* playerItem);

  /**
   * El jugador deja un elemento justo debajo de él. Implica un ascenso del jugador en altura
   * @pre Debe tener un elemento que haya tomado con anterioridad
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void drop(PlayerItem* playerItem);

protected:

  /**
   * Matriz que define el salto del jugador. Cada par de valores define el desplazamiento en el eje
   * X o Y y el desplazamiento en el eje Z en cada ciclo
   */
  std::vector<JumpMotion> jumpMatrix;

  /**
   * Matriz que define el salto largo del jugador: aquel producido por un trampolín o por un conejito
   * de la velocidad. Cada par de valores define el desplazamiento en el eje X o Y y el desplazamiento
   * en el eje Z en cada ciclo
   */
  std::vector<JumpMotion> highJumpMatrix;

  /**
   * Índice de la fase del salto que se está ejecutando
   */
  int jumpIndex;

  /**
   * Número de fases de las que se compone el salto normal
   */
  int jumpFrames;

  /**
   * Número de fases de las que se compone el salto largo
   */
  int highJumpFrames;

  /**
   * Número total de pasos que dará el jugador en modo automático
   */
  int automaticSteps;

  /**
   * Pasos automáticos que le restan por dar al jugador cuando accede a la sala por una puerta
   */
  int automaticStepsCounter;

  /**
   * Número de pasos dado por el jugador a doble velocidad
   */
  int highSpeedSteps;

  /**
   * Número de pasos dado por el jugador con inmunidad
   */
  int shieldSteps;

  /**
   * Fotogramas de caída del jugador. Uno por cada punto cardinal
   */
  int fallFrames[4];

  /**
   * Fotograma en blanco del jugador. Se utiliza durante el cambio de sala a través de un telepuerto
   */
  int nullFrame;

  /**
   * Indica si hay un disparo presente en la sala
   */
  bool shotPresent;

  /**
   * Datos del elemento empleado como transición en el cambio de sala a través de un telepuerto
   */
  short transitionDataLabel;

  /**
   * Datos del elemento empleado como disparo
   */
  short shotDataLabel;

  /**
   * Gestor de datos de los elementos del juego
   */
  ItemDataManager* itemDataManager;

  /**
   * Datos del jugador
   */
  ItemData* playerData;

  /**
   * Datos del elemento tomado por el jugador
   */
  ItemData* takenItemData;

  /**
   * Imagen del elemento tomado por el jugador
   */
  BITMAP* takenItemImage;

  /**
   * Comportamiento del elemento tomado por el jugador
   */
  BehaviorId takenItemBehaviorId;

  /**
   * Cronómetro que controla la velocidad de movimiento del elemento
   */
  HPC* speedTimer;

  /**
   * Cronómetro que controla la velocidad de caída del elemento
   */
  HPC* fallenTimer;

  /**
   * Cronómetro que controla la velocidad al planear del elemento
   */
  HPC* glideTimer;

  /**
   * Cronómetro que controla el parpadeo del elemento
   */
  HPC* blinkingTimer;

public: // Operaciones de consulta y actualización

  /**
   * Número de fases de las que se compone el salto
   */
  int getJumpFrames() const { return jumpFrames; }

  /**
   * Establece la presencia o ausencia de un disparo en la sala
   */
  void setShotPresent(bool isPresent) { this->shotPresent = isPresent; }

  /**
   * Almacena el gestor de datos de los elementos del juego
   * @param data Un puntero a un objeto o estructura de datos
   */
  void setExtraData(void* data);

  /**
   * Establece los datos del elemento que tiene el jugador en el bolso
   * @param data Datos del elemento tomado por el jugador
   * @param takenItemImage Imagen del elemento tomado por el jugador
   * @param behaviorId Comportamiento del elemento tomado por el jugador
   */
  void assignTakenItem(ItemData* itemData, BITMAP* takenItemImage, const BehaviorId& behaviorId);

  /**
   * Datos del elemento tomado por el jugador
   * @return Un registro de datos de un elemento
   */
  ItemData* getTakenItemData() const { return this->takenItemData; }

  /**
   * Imagen del elemento tomado por el jugador
   * @return Una imagen ó 0 si el jugador no tiene ningún elemento
   */
  BITMAP* getTakenItemImage() const { return this->takenItemImage; }

  /**
   * Comportamiento del elemento tomado por el jugador
   * @return Identificador del comportamiento
   */
  BehaviorId getTakenItemBehaviorId() const { return this->takenItemBehaviorId; }
};

}

#endif
