
#include "PlayerItem.hpp"
#include "Behavior.hpp"
#include "ItemData.hpp"
#include "Door.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"
#include "UserControlled.hpp"

namespace isomot
{

PlayerItem::PlayerItem( ItemData* itemData, int x, int y, int z, const Direction& direction )
: FreeItem( itemData, x, y, z, direction ),
        lives( 0 ),
        highSpeed( 0 ),
        highJumps( 0 ),
        shield( 0 ),
        ammo( 0 ),
        exit( NoExit ),
        orientation( NoDirection ),
        shieldTimer( 0 ),
        shieldTime( 25.0 )
{

}

PlayerItem::PlayerItem( const PlayerItem& playerItem )
: FreeItem( playerItem ),
        lives( playerItem.lives ),
        highSpeed( playerItem.highSpeed ),
        highJumps( playerItem.highJumps ),
        shield( playerItem.shield ),
        tools( playerItem.tools ),
        ammo( playerItem.ammo ),
        exit( playerItem.exit ),
        orientation( playerItem.orientation ),
        shieldTimer( 0 ),
        shieldTime( playerItem.shieldTime )
{

}

PlayerItem::~PlayerItem()
{
        if( this->shieldTimer != 0 )
        {
                delete shieldTimer;
        }
}

bool PlayerItem::changeData( int value, int x, int y, int z, const Datum& datum, const WhatToDo& how )
{
        bool collisionFound = false;

        // Vacía la pila de colisiones
        mediator->clearStackOfCollisions( );

        // Copia el elemento antes de realizar el movimiento
        PlayerItem oldPlayerItem( *this );

        switch ( datum )
        {
                case CoordinateX:
                        this->x = value + this->x * how;
                        break;

                case CoordinateY:
                        this->y = value + this->y * how;
                        break;

                case CoordinateZ:
                        this->z = value + this->z * how;
                        break;

                case CoordinatesXYZ:
                        this->x = x + this->x * how;
                        this->y = y + this->y * how;
                        this->z = z + this->z * how;
                        break;

                case WidthX:
                        getDataOfFreeItem()->widthX = value + getDataOfFreeItem()->widthX * how;
                        break;

                case WidthY:
                        getDataOfFreeItem()->widthY = value + getDataOfFreeItem()->widthY * how;
                        break;

                case Height:
                        getDataOfFreeItem()->height = value + getDataOfFreeItem()->height * how;
                        break;
        }

        // Busca colisiones con las puertas de la sala
        bool doorCollision = false;
        if ( ( collisionFound = mediator->findCollisionWithItem( this ) ) )
        {
                // Dirección de avance
                int wayX = ( datum == CoordinateX || x != 0 ? value : 0 );
                int wayY = ( datum == CoordinateY || y != 0 ? value : 0 );

                while ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        int id = mediator->popCollision();

                        // Se ha movido en dirección al muro norte
                        if ( wayX < 0 )
                        {
                                // Si la sala tiene puerta norte se comprueba si ha chocado con alguna jamba
                                if ( ! ( doorCollision = isCollidingWithDoor( North, mediator, id, oldPlayerItem ) ) )
                                {
                                        // Si la sala tiene puerta noreste se comprueba si ha chocado con alguna jamba
                                        if ( ! ( doorCollision = isCollidingWithDoor( Northeast, mediator, id, oldPlayerItem ) ) )
                                        {
                                                // Si la sala tiene puerta noroeste se comprueba si ha chocado con alguna jamba
                                                isCollidingWithDoor( Northwest, mediator, id, oldPlayerItem );
                                        }
                                }
                        }
                        // Se ha movido en dirección al muro sur
                        else if ( wayX > 0 )
                        {
                                // Si la sala tiene puerta sur se comprueba si ha chocado con alguna jamba
                                if ( ! ( doorCollision = isCollidingWithDoor( South, mediator, id, oldPlayerItem ) ) )
                                {
                                        // Si la sala tiene puerta sureste se comprueba si ha chocado con alguna jamba
                                        if ( ! ( doorCollision = isCollidingWithDoor( Southeast, mediator, id, oldPlayerItem ) ) )
                                        {
                                                // Si la sala tiene puerta suroeste se comprueba si ha chocado con alguna jamba
                                                doorCollision = isCollidingWithDoor( Southwest, mediator, id, oldPlayerItem );
                                        }
                                }
                        }
                        // Se ha movido en dirección al muro este
                        else if ( wayY < 0 )
                        {
                                // Si la sala tiene puerta este se comprueba si ha chocado con alguna jamba
                                if ( ! ( doorCollision = isCollidingWithDoor( East, mediator, id, oldPlayerItem ) ) )
                                {
                                        // Si la sala tiene puerta este-norte se comprueba si ha chocado con alguna jamba
                                        if ( ! ( doorCollision = isCollidingWithDoor( Eastnorth, mediator, id, oldPlayerItem ) ) )
                                        {
                                                // Si la sala tiene puerta este-sur se comprueba si ha chocado con alguna jamba
                                                isCollidingWithDoor( Eastsouth, mediator, id, oldPlayerItem );
                                        }
                                }
                        }
                        // Se ha movido en dirección al muro oeste
                        else if ( wayY > 0 )
                        {
                                // Si la sala tiene puerta este se comprueba si ha chocado con alguna jamba
                                if ( ! ( doorCollision = isCollidingWithDoor( West, mediator, id, oldPlayerItem ) ) )
                                {
                                        // Si la sala tiene puerta este-norte se comprueba si ha chocado con alguna jamba
                                        if ( ! ( doorCollision = isCollidingWithDoor( Westnorth, mediator, id, oldPlayerItem ) ) )
                                        {
                                                // Si la sala tiene puerta este-sur se comprueba si ha chocado con alguna jamba
                                                isCollidingWithDoor( Westsouth, mediator, id, oldPlayerItem );
                                        }
                                }
                        }
                }
        }

        // Se buscan colisiones con las paredes reales, aquellas que delimitan la sala
        if ( this->x < mediator->getBound( North )
                && isNotUnderDoor( North, mediator ) && isNotUnderDoor( Northeast, mediator ) && isNotUnderDoor( Northwest, mediator ) )
        {
                mediator->pushCollision( NorthWall );
        }
        else if ( this->x + getDataOfFreeItem()->widthX > mediator->getBound( South )
                && isNotUnderDoor( South, mediator ) && isNotUnderDoor( Southeast, mediator ) && isNotUnderDoor( Southwest, mediator ) )
        {
                mediator->pushCollision( SouthWall );
        }

        if ( this->y - getDataOfFreeItem()->widthY + 1 < mediator->getBound( East )
                && isNotUnderDoor( East, mediator ) && isNotUnderDoor( Eastnorth, mediator ) && isNotUnderDoor( Eastsouth, mediator ) )
        {
                mediator->pushCollision( EastWall );
        }
        else if ( this->y >= mediator->getBound( West )
                && isNotUnderDoor( West, mediator ) && isNotUnderDoor( Westnorth, mediator ) && isNotUnderDoor( Westsouth, mediator ) )
        {
                mediator->pushCollision( WestWall );
        }

        // Si ha habido colisión con alguna pared, se detiene el proceso
        if ( ! ( collisionFound = ! mediator->isStackOfCollisionsEmpty () ) )
        {
                // Se buscan colisiones con los límites de la sala, ahora que se sabe que el jugador
                // puede estar atravesando una puerta
                Direction directions[] = {  Northeast, Northwest, North, Southeast, Southwest, South,
                                                Eastnorth, Eastsouth, East, Westnorth, Westsouth, West  };

                SpecialId borders[] = { NortheastBorder, NorthwestBorder, NorthBorder,
                                        SoutheastBorder, SouthwestBorder, SouthBorder,
                                        EastnorthBorder, EastsouthBorder, EastBorder,
                                        WestnorthBorder, WestsouthBorder, WestBorder };

                // Se comprueba cada límite de la sala en el orden almacenado en los arrays

                // Primero el grupo norte/sur
                for ( int i = 0; i < 6; i++ )
                {
                        if ( isCollidingWithRoomBorder( directions[ i ], mediator ) )
                        {
                                mediator->pushCollision( borders[ i ] );
                                // No hace falta seguir comprobando límites
                                break;
                        }
                }

                // Luego el grupo este/oeste
                for ( int i = 6; i < 12; i++ )
                {
                        if ( isCollidingWithRoomBorder( directions[ i ], mediator ) )
                        {
                                mediator->pushCollision( borders[ i ] );
                                // No hace falta seguir comprobando límites
                                break;
                        }
                }

                // Por último se buscan colisiones con el suelo
                if ( this->z < 0 )
                {
                        mediator->pushCollision( Floor );
                }

                // Si se ha llegado a los límites de la sala se detiene el proceso
                if ( ! ( collisionFound = ! mediator->isStackOfCollisionsEmpty () ) )
                {
                        // Busca colisiones con el resto de elementos de la sala
                        // Si hay colisión se interrumpe el proceso
                        if ( ! ( collisionFound = mediator->findCollisionWithItem( this ) ) )
                        {
                                // Si el elemento tiene imagen se marcan para enmascarar los elementos
                                // libres cuyas imágenes se solapen con la suya. La operación se realiza
                                // tanto para la posición anterior como la posición actual
                                if ( this->image )
                                {
                                        // A cuántos píxeles está la imagen del punto origen de la sala
                                        this->offset.first = ( ( this->x - this->y ) << 1 ) + getDataOfFreeItem()->widthX + getDataOfFreeItem()->widthY - ( image->w >> 1 ) - 1;
                                        this->offset.second = this->x + this->y + getDataOfFreeItem()->widthX - image->h - this->z;

                                        mediator->markItemsForMasking( &oldPlayerItem );
                                        mediator->markItemsForMasking( this );
                                }
                                else
                                {
                                        this->offset.first = this->offset.second = 0;
                                }

                                // Si las sombras están activas se buscan qué elementos hay que volver a sombrear
                                // La búsqueda se realiza tanto para la posición anterior como la posición actual
                                if ( mediator->getDegreeOfShading() < 256 )
                                {
                                        mediator->markItemsForShady( &oldPlayerItem );
                                        mediator->markItemsForShady( this );
                                }

                                // El elemento debe volver a sombrearse y enmascararse tras el cambio de posición
                                this->myShady = WantShadow;
                                this->myMask = WantMask;

                                // La lista de elementos libres debe reordenarse
                                mediator->activateFreeItemsSorting();
                        }
                }
        }

        // Si hubo colisión se restauran los valores anteriores
        if ( collisionFound && ! doorCollision )
        {
                this->x = oldPlayerItem.getX();
                this->y = oldPlayerItem.getY();
                this->z = oldPlayerItem.getZ();
                this->dataOfItem->widthX = oldPlayerItem.getWidthX();
                this->dataOfItem->widthY = oldPlayerItem.getWidthY();
                this->dataOfItem->height = oldPlayerItem.getHeight();
                this->offset = oldPlayerItem.getOffset();
        }

        return ! collisionFound ;
}

bool PlayerItem::isCollidingWithDoor( const Direction& direction, Mediator* mediator, int id, const PlayerItem& previousPosition )
{
        Door* door = mediator->getDoor( direction );
        int oldX = this->x;
        int oldY = this->y;

        // Acceso a la puerta
        switch( direction )
        {
                case North:
                case Northeast:
                case Northwest:
                case South:
                case Southeast:
                case Southwest:
                        // Si la sala tiene puerta norte o sur se comprueba si ha chocado con alguna jamba
                        if( door )
                        {
                                // Si choca contra la jamba izquierda desplaza al jugador a la derecha
                                if( door->getLeftJamb()->getId() == id && this->y <= door->getLeftJamb()->getY() )
                                {
                                        this->y--;
                                        this->x = previousPosition.getX();
                                }
                                // Si choca contra la jamba derecha desplaza al jugador a la izquierda
                                else if( door->getRightJamb()->getId() == id && this->y - getDataOfFreeItem()->widthY >= door->getRightJamb()->getY() - door->getRightJamb()->getWidthY() )
                                {
                                        this->y++;
                                        this->x = previousPosition.getX();
                                }
                        }
                        break;

                case East:
                case Eastnorth:
                case Eastsouth:
                case West:
                case Westnorth:
                case Westsouth:
                        // Si la sala tiene puerta este u oeste se comprueba si ha chocado con alguna jamba
                        if( door )
                        {
                                // Si choca contra la jamba izquierda desplaza al jugador a la derecha
                                if( door->getLeftJamb()->getId() == id && this->x >= door->getLeftJamb()->getX() )
                                {
                                        this->x++;
                                        this->y = previousPosition.getY();
                                }
                                // Si choca contra la jamba derecha desplaza al jugador a la izquierda
                                else if( door->getRightJamb()->getId() == id && this->x - getDataOfFreeItem()->widthX <= door->getRightJamb()->getX() + door->getRightJamb()->getWidthX() )
                                {
                                        this->x--;
                                        this->y = previousPosition.getY();
                                }
                        }
                        break;

                default:
                        ;
        }

        return ( oldX != this->x || oldY != this->y );
}

bool PlayerItem::isNotUnderDoor( const Direction& direction, Mediator* mediator )
{
        bool result = false;
        Door* door = mediator->getDoor( direction );

        switch( direction )
        {
                case North:
                case Northeast:
                case Northwest:
                case East:
                case Eastnorth:
                case Eastsouth:
                case South:
                case Southeast:
                case Southwest:
                case West:
                case Westnorth:
                case Westsouth:
                        result = ( ! door || ( door && !door->isUnderDoor( this->x, this->y, this->z ) ) );
                        break;

                default:
                        ;
        }

        return result;
}

bool PlayerItem::isCollidingWithRoomBorder( const Direction& direction, Mediator* mediator )
{
        bool result = false;
        Door* door = mediator->getDoor( direction );

        switch( direction )
        {
                case North:
                        result = ( this->x < 0 );
                        break;

                case Northeast:
                case Northwest:
                        result = ( door && this->x < mediator->getBound( direction ) && door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case South:
                        result = ( this->x + getDataOfFreeItem()->widthX > mediator->getTilesX() * mediator->getTileSize() );
                        break;

                case Southeast:
                case Southwest:
                        result = ( door && this->x + getDataOfFreeItem()->widthX > mediator->getBound( direction ) && door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case East:
                        result = ( this->y - getDataOfFreeItem()->widthY + 1 < 0 );
                        break;

                case Eastnorth:
                case Eastsouth:
                        result = ( door && this->y - getDataOfFreeItem()->widthY + 1 < mediator->getBound( direction ) && door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case West:
                        result = ( this->y >= mediator->getTilesY() * mediator->getTileSize() );
                        break;

                case Westnorth:
                case Westsouth:
                        result = ( door && this->y + getDataOfFreeItem()->widthY > mediator->getBound( direction ) && door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                default:
                        ;
        }

        return result;
}

void PlayerItem::behave ()
{
        UserControlled* userBehavior = dynamic_cast< UserControlled* >( this->behavior );
        if ( userBehavior != 0 )
        {
                userBehavior->behave ( );
        }
}

bool PlayerItem::update ()
{
        if ( behavior != 0 )
        {
                behavior->update ( );
        }

        return false;
}

void PlayerItem::wait ()
{
        StateId stateId = this->behavior->getStateId();

        // Si el jugador se está teletransportando o está muriendo no se podrá detener
        if ( stateId != StateStartWayOutTeletransport && stateId != StateWayOutTeletransport &&
                stateId != StateStartWayInTeletransport && stateId != StateWayInTeletransport &&
                stateId != StateStartDestroy && stateId != StateDestroy )
        {
                // El fotograma de parada es distinto segn la orientación del elemento
                int currentFrame = ( getDataOfFreeItem()->motion.size() - getDataOfFreeItem()->extraFrames ) / getDataOfFreeItem()->directionFrames * static_cast < int > ( direction );

                // Si la imagen actual es distinta a la nueva imagen entonces se cambia
                if ( this->image != 0 && this->image != getDataOfFreeItem ()->motion[ currentFrame ] )
                {
                        changeImage( getDataOfFreeItem ()->motion[ currentFrame ] );

                        if ( this->shadow != 0 )
                        {
                                changeShadow( getDataOfFreeItem ()->shadows[ currentFrame ] );
                        }
                }

                this->behavior->changeStateId( StateWait );
        }
}

void PlayerItem::addLives( unsigned char lives )
{
        if( this->lives < 100 )
        {
                this->lives += lives;
                GameManager::getInstance()->addLives( PlayerId( this->getLabel () ), lives );
        }
}

void PlayerItem::loseLife()
{
        if( this->lives > 0 )
        {
                this->lives--;
                GameManager::getInstance()->loseLife( PlayerId( this->getLabel () ) );
        }
        GameManager::getInstance()->emptyHandbag( PlayerId( this->getLabel () ) );
}

void PlayerItem::takeTool( short label )
{
        this->tools.push_back( label );
        GameManager::getInstance()->takeTool( MagicItem( label ) );
}

void PlayerItem::addAmmo( const unsigned short ammo )
{
        this->ammo += ammo;
        GameManager::getInstance()->setDonuts( this->ammo );
}

void PlayerItem::consumeAmmo()
{
        if( this->ammo > 0 )
        {
                this->ammo--;
                GameManager::getInstance()->consumeDonut();
        }
}

void PlayerItem::activateHighSpeed()
{
  this->highSpeed = 99;
  GameManager::getInstance()->addHighSpeed( PlayerId( this->getLabel() ), 99 );
}

void PlayerItem::decreaseHighSpeed()
{
        if( this->highSpeed > 0 )
        {
                this->highSpeed--;
                GameManager::getInstance()->decreaseHighSpeed( PlayerId( this->getLabel() ) );
        }
}

void PlayerItem::addHighJumps( unsigned char highJumps )
{
        this->highJumps += highJumps;
        GameManager::getInstance()->addHighJumps( PlayerId( this->getLabel() ), highJumps );
}

void PlayerItem::decreaseHighJumps()
{
        if( this->highJumps > 0 )
        {
                this->highJumps--;
                GameManager::getInstance()->decreaseHighJumps( PlayerId( this->getLabel() ) );
        }
}

void PlayerItem::activateShield()
{
        this->shieldTimer = new HPC();
        this->shieldTimer->start();
        this->shieldTime = 25.000;
        this->shield = shieldTime - shieldTimer->getValue();
        GameManager::getInstance()->addShield( PlayerId( this->getLabel() ), this->shield );
}

void PlayerItem::decreaseShield()
{
        if( this->shieldTimer != 0 )
        {
                this->shield = shieldTime - shieldTimer->getValue();
                GameManager::getInstance()->decreaseShield( PlayerId( this->getLabel() ), this->shield );
        }

        if( this->shield < 0 )
        {
                this->shieldTime = 0.000;
                this->shield = 0;
                this->shieldTimer->stop();
                delete this->shieldTimer;
                this->shieldTimer = 0;
        }
}

void PlayerItem::liberatePlanet ()
{
        std::string scenery = this->mediator->getRoom()->getScenery();

        if( scenery.compare( "jail" ) == 0 || scenery.compare( "blacktooth" ) == 0 || scenery.compare( "market" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( Blacktooth );
        }
        else if( scenery.compare( "egyptus" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( Egyptus );
        }
        else if( scenery.compare( "penitentiary" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( Penitentiary );
        }
        else if( scenery.compare( "safari" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( Safari );
        }
        else if( scenery.compare( "byblos" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( Byblos );
        }
}

void PlayerItem::assignTakenItem ( ItemData* itemData, BITMAP* takenItemImage, const BehaviorId& behaviorId )
{
        dynamic_cast< UserControlled* >( this->behavior )->assignTakenItem( itemData, takenItemImage, behaviorId );
}

ItemData* PlayerItem::consultTakenItem ( BehaviorId* behaviorId )
{
        *behaviorId = dynamic_cast< UserControlled* >( this->behavior )->getTakenItemBehaviorId();
        return dynamic_cast< UserControlled* >( this->behavior )->getTakenItemData();
}

BITMAP* PlayerItem::consultTakenItemImage ()
{
        return dynamic_cast< UserControlled* >( this->behavior )->getTakenItemImage();
}

void PlayerItem::save ()
{
        GameManager::getInstance()->eatFish( this, this->mediator->getRoom() );
}

void PlayerItem::saveAt ( int x, int y, int z )
{
        GameManager::getInstance()->eatFish( this, this->mediator->getRoom(), x, y, z );
}

void PlayerItem::setShieldTime ( double shield )
{
        this->shieldTime = shield;

        if( this->shieldTime > 0 && this->shieldTimer == 0 )
        {
                this->shieldTimer = new HPC();
                this->shieldTimer->start();
        }
}

bool PlayerItem::hasTool( const MagicItem& label ) const
{
        return std::find( this->tools.begin (), this->tools.end (), short( label ) ) != this->tools.end ();
}

}
