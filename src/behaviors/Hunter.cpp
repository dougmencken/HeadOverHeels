
#include "Hunter.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"


namespace isomot
{

Hunter::Hunter( Item * item, const BehaviorOfItem & id ) :
        Behavior( item, id )
{
        speedTimer = new HPC();
        speedTimer->start();
}

Hunter::~Hunter()
{
        delete speedTimer;
}

bool Hunter::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();
        Mediator* mediator = freeItem->getMediator();
        bool alive = true;

        switch ( activity )
        {
                case Wait:
                // Si el elemento no tiene que esperar la cercanía del jugador, se activa sin más
                if ( theBehavior == Hunter4Behavior || theBehavior == Hunter8Behavior )
                {
                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                        activity = calculateDirection( activity );
                }
                // Se comprueba la cercanía del jugador para activar el elemento
                else
                {
                        int delta = mediator->getTileSize() * 3;

                        // Si el jugador está dentro del rectángulo definido en torno al cazador entonces el cazador se activa
                        if ( playerItem != 0  &&
                                playerItem->getX() > freeItem->getX() - delta  &&
                                playerItem->getX() < freeItem->getX() + freeItem->getWidthX() + delta  &&
                                playerItem->getY() > freeItem->getY() - delta  &&
                                playerItem->getY() < freeItem->getY() + freeItem->getWidthY() + delta )
                        {
                                activity = calculateDirection( activity );
                        }

                        // Si se mueve en ocho direcciones emite sonido cuando está detenido
                        if ( theBehavior == HunterWaiting8Behavior )
                        {
                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                        }

                        // Anima el elemento aunque esté detenido
                        freeItem->animate();
                }
                break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                        // Si se crea el guarda completo entonces el elemento actual debe destruirse
                        if ( theBehavior == HunterWaiting4Behavior && createFullBody() )
                        {
                                alive = false;
                        }
                        // item is active and it is time to move
                        else if ( ! freeItem->isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        // El elemento se mueve
                                        whatToDo->move( this, &activity, false );
                                        // Se pone a cero el cronómetro para el siguiente ciclo
                                        speedTimer->reset();
                                        // Comprueba si hay cambio de dirección
                                        activity = calculateDirection( activity );
                                        // Cae si tiene que hacerlo
                                        if ( freeItem->getWeight() > 0 )
                                        {
                                                FallKindOfActivity::getInstance()->fall( this );
                                        }
                                }

                                // Anima el elemento
                                freeItem->animate();

                                // Emite el sonido de movimiento
                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                        }
                        break;

                case MoveNortheast:
                case MoveNorthwest:
                case MoveSoutheast:
                case MoveSouthwest:
                        if ( ! freeItem->isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        // El elemento se mueve
                                        if ( ! whatToDo->move( this, &activity, false ) )
                                        {
                                                if ( activity == MoveNortheast || activity == MoveNorthwest )
                                                {
                                                        ActivityOfItem tempActivity = MoveNorth;
                                                        if ( ! whatToDo->move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == MoveNortheast ? MoveEast : MoveWest );
                                                                if ( freeItem->getWeight() > 0 )
                                                                {
                                                                        FallKindOfActivity::getInstance()->fall( this );
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        ActivityOfItem tempActivity = MoveSouth;
                                                        if ( ! whatToDo->move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == MoveSoutheast ? MoveEast : MoveWest );
                                                                if ( freeItem->getWeight() > 0 )
                                                                {
                                                                        FallKindOfActivity::getInstance()->fall( this );
                                                                }
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                // Comprueba si hay cambio de dirección
                                                activity = calculateDirection( activity );
                                        }

                                        // Se pone a cero el cronómetro para el siguiente ciclo
                                        speedTimer->reset();
                                }

                                // Anima el elemento
                                freeItem->animate();

                                // Emite el sonido de movimiento
                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                        }
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceNorthwest:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                        // Si el elemento está activo y ha llegado el momento de moverse, entonces:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // El elemento se mueve
                                whatToDo->displace( this, &activity, false );
                                activity = Wait;
                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();
                        }

                        // preserve inactivity for frozen item
                        if ( freeItem->isFrozen() )
                        {
                                activity = Freeze;
                        }
                        break;

                case Freeze:
                        freeItem->setFrozen( true );
                        break;

                case WakeUp:
                        freeItem->setFrozen( false );
                        activity = Wait;
                        break;

                default:
                        ;
        }

        return !alive;
}

ActivityOfItem Hunter::calculateDirection( const ActivityOfItem& activity )
{
        if ( theBehavior == Hunter4Behavior || theBehavior == HunterWaiting4Behavior )
        {
                return calculateDirection4( activity );
        }
        else if ( theBehavior == Hunter8Behavior || theBehavior == HunterWaiting8Behavior )
        {
                return calculateDirection8( activity );
        }

        return Wait;
}

ActivityOfItem Hunter::calculateDirection4( const ActivityOfItem& activity )
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();

        // Se comprueba si el jugador activo está en la sala
        if ( playerItem != 0 )
        {
                int dx = freeItem->getX() - playerItem->getX();
                int dy = freeItem->getY() - playerItem->getY();

                if ( abs( dy ) > abs( dx ) )
                {
                        // Se moverá al este o al oeste porque la distancia X al jugador es menor
                        if ( dx > 0 )
                        {
                                // Si la distancia es positiva se mueve al norte
                                changeActivityOfItem( MoveNorth );
                        }
                        else if ( dx < 0 )
                        {
                                // Si la distancia es negativa se mueve al sur
                                changeActivityOfItem( MoveSouth );
                        }
                        else
                        {
                                if ( dy > 0 )
                                {
                                        // Si la distancia es positiva se mueve al este
                                        changeActivityOfItem( MoveEast );
                                }
                                else if ( dy < 0 )
                                {
                                        // Si la distancia es negativa se mueve al oeste
                                        changeActivityOfItem( MoveWest );
                                }
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        // Se moverá al norte o al sur porque la distancia Y al jugador es menor
                        if ( dy > 0 )
                        {
                                // Si la distancia es positiva se mueve al este
                                changeActivityOfItem( MoveEast );
                        }
                        else if ( dy < 0 )
                        {
                                // Si la distancia es negativa se mueve al oeste
                                changeActivityOfItem( MoveWest );
                        }
                        else
                        {
                                if ( dx > 0 )
                                {
                                        // Si la distancia es positiva se mueve al norte
                                        changeActivityOfItem( MoveNorth );
                                }
                                else if ( dx < 0 )
                                {
                                        // Si la distancia es negativa se mueve al sur
                                        changeActivityOfItem( MoveSouth );
                                }
                        }
                }
        }

        return activity;
}

ActivityOfItem Hunter::calculateDirection8( const ActivityOfItem& activity )
{
        FreeItem* freeItem = dynamic_cast< FreeItem* >( this->item );
        PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();

        // Se comprueba si el jugador activo está en la sala
        if ( playerItem != 0 )
        {
                // Distancia del cazador al jugador en los ejes X e Y
                int dx = freeItem->getX() - playerItem->getX();
                int dy = freeItem->getY() - playerItem->getY();

                // El siguiente algoritmo obtiene la dirección más adecuada, aquella que permita alcanzar al
                // jugador lo más rápido posible, en función de las distancias en los ejes X e Y entre el
                // cazador y el jugador
                if ( abs(dy) > abs(dx) )
                {
                        if ( dx > 1 )
                        {
                                if ( dy > 1 )
                                        changeActivityOfItem( MoveNortheast );
                                else if ( dy < -1 )
                                        changeActivityOfItem( MoveNorthwest );
                                else
                                        changeActivityOfItem( MoveNorth );
                        }
                        else if ( dx < -1 )
                        {
                                if ( dy > 1 )
                                        changeActivityOfItem( MoveSoutheast );
                                else if ( dy < -1 )
                                        changeActivityOfItem( MoveSouthwest );
                                else
                                        changeActivityOfItem( MoveSouth );
                        }
                        else
                        {
                                if ( dy > 0 )
                                        changeActivityOfItem( MoveEast );
                                else if ( dy < 0 )
                                        changeActivityOfItem( MoveWest );
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 1 )
                        {
                                if ( dx > 1 )
                                        changeActivityOfItem( MoveNortheast );
                                else if ( dx < -1 )
                                        changeActivityOfItem( MoveSoutheast );
                                else
                                        changeActivityOfItem( MoveEast );
                        }
                        else if ( dy < -1 )
                        {
                                if ( dx > 1 )
                                        changeActivityOfItem( MoveNorthwest );
                                else if ( dx < -1 )
                                        changeActivityOfItem( MoveSouthwest );
                                else
                                        changeActivityOfItem( MoveWest );
                        }
                        else
                        {
                                if ( dx > 0 )
                                        changeActivityOfItem( MoveNorth );
                                else if ( dx < 0 )
                                        changeActivityOfItem( MoveSouth );
                        }
                }

                // guardian of throne flees from player with four crowns
                if ( item->getLabel() == "throne-guard" && GameManager::getInstance()->countFreePlanets() >= 4 )
                {
                        changeActivityOfItem( MoveSouthwest );
                }
        }

        return activity;
}

bool Hunter::createFullBody()
{
        FreeItem* freeItem = dynamic_cast< FreeItem* >( this->item );
        bool created = false;

        if ( freeItem->getLabel() == "imperial-guard-head" && freeItem->checkPosition( 0, 0, -LayerHeight, Add ) )
        {
                created = true;

                // Crea el elemento en la misma posición que el jugador y a su misma altura
                FreeItem* newItem = new FreeItem( guardData,
                                                  freeItem->getX(), freeItem->getY(), freeItem->getZ() - LayerHeight,
                                                  freeItem->getDirection() );

                newItem->assignBehavior( Hunter4Behavior, 0 );

                // El elemento actual debe dejar de detectar colisiones porque,
                // de lo contrariom no se podrá crear el guarda completo
                freeItem->setCollisionDetector( false );

                // Se añade a la sala actual
                freeItem->getMediator()->getRoom()->addItem( newItem );
        }

        return created;
}

void Hunter::setMoreData( void * data )
{
        this->guardData = reinterpret_cast< ItemData * >( data );
}

}
