
#include "UserControlled.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "ItemDataManager.hpp"
#include "PlayerItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "JumpKindOfActivity.hpp"
#include "FireDoughnut.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

UserControlled::UserControlled( Item * item, const BehaviorOfItem & id )
        : Behavior( item, id )
        , jumpIndex( 0 )
        , jumpFrames( 0 )
        , highJumpFrames( 0 )
        , automaticSteps( 0 )
        , automaticStepsCounter( 0 )
        , highSpeedSteps( 0 )
        , shieldSteps( 0 )
        , takenItemData( 0 )
        , takenItemImage( 0 )
        , takenItemBehavior( NoBehavior )
        , speedTimer( 0 )
        , fallTimer( 0 )
        , glideTimer( 0 )
        , blinkingTimer( 0 )
{
        fallFrames[ North ] = fallFrames[ South ] = fallFrames[ East ] = fallFrames[ West ] = 0xffffffff; // wtf is this magic mask for ?
}

UserControlled::~UserControlled()
{

}

void UserControlled::changeActivityOfItem( const ActivityOfItem & activityOf, Item * sender )
{
        this->activity = activityOf;
        this->sender = sender;

        // Asigna el estado en función del identificador
        switch ( activityOf )
        {
                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                case MoveNortheast:
                case MoveNorthwest:
                case MoveSoutheast:
                case MoveSouthwest:
                case MoveUp:
                case MoveDown:
                        whatToDo = MoveKindOfActivity::getInstance();
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                case ForceDisplaceNorth:
                case ForceDisplaceSouth:
                case ForceDisplaceEast:
                case ForceDisplaceWest:
                        whatToDo = DisplaceKindOfActivity::getInstance();
                        break;

                case Fall:
                        whatToDo = FallKindOfActivity::getInstance();
                        break;

                case StartWayInTeletransport:
                        this->item->changeFrame( this->nullFrame );
                        break;

                default:
                        ;
        }
}

void UserControlled::wait( PlayerItem * player )
{
        player->wait();

        // Se comprueba si el jugador debe empezar a caer
        if ( FallKindOfActivity::getInstance()->fall( this ) )
        {
                speedTimer->reset();
                activity = Fall;

                // Si el jugador se detiene entonces se para la cuenta atrás
                // del conejo que proporciona doble velocidad
                if ( player->getLabel() == WhichPlayer( Head ) && player->getHighSpeed() > 0 && this->highSpeedSteps < 4 )
                {
                        this->highSpeedSteps = 0;
                }
        }
}

void UserControlled::move( PlayerItem * player )
{
        // move when the item is active
        if ( ! player->isFrozen() )
        {
                // Si el jugador ha cogido velocidad extra entonces se divide su velocidad habitual
                double speed = player->getSpeed() / ( player->getLabel() == WhichPlayer( Head ) && player->getHighSpeed() > 0 ? 2 : 1 );

                // Si ha llegado el momento de moverse
                if ( speedTimer->getValue() > speed )
                {
                        whatToDo = MoveKindOfActivity::getInstance();

                        // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
                        // se desplaza a los elementos con los que pudiera haber chocado
                        bool moved = whatToDo->move( this, &activity, true );

                        // Si se ha movido y no ha cambiado de estado entonces se cuenta un paso
                        if ( player->getLabel() == WhichPlayer( Head ) && player->getHighSpeed() > 0 && moved && activity != Fall )
                        {
                                this->highSpeedSteps++;

                                if ( this->highSpeedSteps == 8 )
                                {
                                        player->decreaseHighSpeed();
                                        this->highSpeedSteps = 4;
                                }
                        }

                        // Se pone a cero el cronómetro para el siguiente ciclo
                        speedTimer->reset();

                        // Anima el elemento
                        player->animate();
                }
        }
}

void UserControlled::autoMove( PlayerItem * player )
{
        // Si el jugador ha cogido velocidad extra entonces se divide su velocidad habitual
        double speed = player->getSpeed() / ( player->getLabel() == WhichPlayer( Head ) && player->getHighSpeed() > 0 ? 2 : 1 );

        // Si el elemento está activo y ha llegado el momento de moverse, entonces:
        if ( speedTimer->getValue() > speed )
        {
                whatToDo = MoveKindOfActivity::getInstance();

                // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
                // se desplaza a los elementos con los que pudiera haber chocado
                whatToDo->move( this, &activity, true );

                // Se pone a cero el cronómetro para el siguiente ciclo
                speedTimer->reset();

                // Anima el elemento
                player->animate();

                // Se comprueba si seguirá moviéndose de forma automática
                if ( --automaticStepsCounter < 0 )
                {
                        automaticStepsCounter = automaticSteps;
                        activity = Wait;
                }
        }

        // Si deja de moverse de forma automática se detiene el sonido correspondiente
        if ( activity == Wait )
        {
                SoundManager::getInstance()->stop( player->getLabel(), AutoMove );
        }
}

void UserControlled::displace( PlayerItem * player )
{
        // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
        // colisión entonces el estado se propaga a los elementos con los que ha chocado
        if ( speedTimer->getValue() > player->getSpeed() )
        {
                whatToDo->displace( this, &activity, true );
                // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                activity = Wait;
                // Se pone a cero el cronómetro para el siguiente ciclo
                speedTimer->reset();
        }
}

void UserControlled::cancelDisplace( PlayerItem * player )
{
        if ( ! player->isFrozen() )
        {
                if ( speedTimer->getValue() > player->getSpeed() )
                {
                        whatToDo = MoveKindOfActivity::getInstance();

                        // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
                        // se desplaza a los elementos con los que pudiera haber chocado
                        whatToDo->move( this, &activity, false );

                        // Se pone a cero el cronómetro para el siguiente ciclo
                        speedTimer->reset();

                        // Anima el elemento
                        player->animate();
                }
        }
}

void UserControlled::fall( PlayerItem * player )
{
        // Si ha llegado el momento de caer entonces el elemento desciende una unidad
        if ( fallTimer->getValue() > player->getWeight() )
        {
                whatToDo = FallKindOfActivity::getInstance();

                // Si no hay colisión ahora ni en el siguiente ciclo, selecciona el fotograma de caída del personaje
                if ( whatToDo->fall( this ) )
                {
                        if ( player->checkPosition( 0, 0, -1, Add ) )
                        {
                                player->changeFrame( fallFrames[ player->getDirection() ] );
                        }
                }
                // Si hay colisión deja de caer y vuelve al estado inicial siempre que
                // el jugador no haya sido destruido por la colisión con un elemento mortal
                else if ( activity != StartDestroy || ( activity == StartDestroy && player->getShieldTime() > 0 ) )
                {
                        activity = Wait;
                }

                // Se pone a cero el cronómetro para el siguiente ciclo
                fallTimer->reset();
        }

        // Si deja de caer se detiene el sonido correspondiente
        if ( activity != Fall )
        {
                SoundManager::getInstance()->stop( player->getLabel(), Fall );
        }
}

void UserControlled::jump( PlayerItem * player )
{
        switch ( activity )
        {
                case Jump:
                        // Almacena en la pila de colisiones los elementos que tiene debajo
                        player->checkPosition( 0, 0, -1, Add );
                        // Si está sobre un trampolín o tiene el conejo de los grandes saltos, el jugador dará un gran salto
                        activity = ( player->getMediator()->collisionWithByBehavior( TrampolineBehavior ) ||
                                        ( player->getHighJumps() > 0 && player->getLabel() == WhichPlayer( Heels ) )
                                ? HighJump
                                : RegularJump );
                        // Si está sobre el trampolín emite el sonido propio de este elemento
                        if ( activity == HighJump )
                        {
                                if ( player->getLabel() == WhichPlayer( Heels ) )
                                {
                                        player->decreaseHighJumps();
                                }
                                SoundManager::getInstance()->play( player->getLabel(), Rebound );
                        }
                        break;

                case RegularJump:
                        // Si ha llegado el momento de saltar:
                        if ( speedTimer->getValue() > player->getSpeed() )
                        {
                                // Salta en función del ciclo actual
                                whatToDo = JumpKindOfActivity::getInstance();
                                whatToDo->jump( this, &activity, jumpMatrix, &jumpIndex );

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();

                                // Anima el elemento
                                player->animate();
                        }
                        break;

                case HighJump:
                        // Si ha llegado el momento de saltar:
                        if ( speedTimer->getValue() > player->getSpeed() )
                        {
                                // Salta en función del ciclo actual
                                whatToDo = JumpKindOfActivity::getInstance();
                                whatToDo->jump( this, &activity, highJumpMatrix, &jumpIndex );

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();

                                // Anima el elemento
                                player->animate();
                        }
                        break;

                default:
                        ;
        }

        // Si deja de saltar se detiene el sonido correspondiente
        if ( activity != Jump && activity != RegularJump && activity != HighJump )
        {
                SoundManager::getInstance()->stop( player->getLabel(), Jump );
        }

        // Si el jugador supera la altura máxima de la sala entonces pasa a la sala de arriba
        // Se supone que no hay posibilidad de alcanzar la altura máxima de una sala que no
        // conduce a otra situada sobre ella
        if ( player->getZ() >= MaxLayers * LayerHeight )
        {
                player->setExit( Up );
                player->setOrientation( player->getDirection() );
        }
}

void UserControlled::glide( PlayerItem * player )
{
        // Si ha pasado el tiempo necesario para que el elemento descienda:
        //if ( glideTimer->getValue() > player->getSpeed() / 2.0 )
        if ( glideTimer->getValue() > player->getWeight() )
        {
                whatToDo = FallKindOfActivity::getInstance();

                // Si hay colisión deja de caer y vuelve al estado inicial
                if ( ! whatToDo->fall( this ) && ( activity != StartDestroy || ( activity == StartDestroy && player->getShieldTime() > 0 ) ) )
                {
                        activity = Wait;
                }

                // Se pone a cero el cronómetro para el siguiente ciclo
                glideTimer->reset();
        }

        // Si ha pasado el tiempo necesario para mover el elemento:
        if ( speedTimer->getValue() > player->getSpeed() * ( player->getLabel() == WhichPlayer( HeadAndHeels ) ? 2 : 1 ) )
        {
                whatToDo = MoveKindOfActivity::getInstance();
                ActivityOfItem subactivity;

                // Subestado para ejecutar el movimiento
                switch ( player->getDirection() )
                {
                        case North:
                                subactivity = MoveNorth;
                                break;

                        case South:
                                subactivity = MoveSouth;
                                break;

                        case East:
                                subactivity = MoveEast;
                                break;

                        case West:
                                subactivity = MoveWest;
                                break;

                        default:
                                ;
                }

                // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
                // se desplaza a los elementos con los que pudiera haber chocado y el elemento da media
                // vuelta cambiando su estado a otro de movimiento
                whatToDo->move( this, &subactivity, false );

                // Selecciona el fotograma de caída del personaje
                player->changeFrame( fallFrames[ player->getDirection() ] );

                // Se pone a cero el cronómetro para el siguiente ciclo
                speedTimer->reset();
        }

        // Si deja de planear se detiene el sonido correspondiente
        if ( activity != Glide )
        {
                SoundManager::getInstance()->stop( player->getLabel(), Glide );
        }
}

void UserControlled::wayInTeletransport( PlayerItem * player )
{
        switch ( activity )
        {
                case StartWayInTeletransport:
                        // Almacena la orientación de entrada
                        player->setOrientation( player->getDirection() );
                        // Cambia a las burbujas pero conservando la etiqueta del jugador
                        playerData = player->getDataOfFreeItem() ;
                        player->changeItemData( itemDataManager->findItemByLabel( transitionDataLabel ), NoDirection );
                        // Las burbujas se animarán marcha atrás
                        player->setBackwardMotion();
                        // Inicia el teletransporte
                        activity = WayInTeletransport;
                        break;

                case WayInTeletransport:
                        // Anima el elemento. Si ha llegado al final el jugador aparece
                        if ( player->animate() )
                        {
                                // Cambia al aspecto original del jugador
                                player->changeItemData( playerData, NoDirection );
                                // Se recupera su orientación original
                                player->changeDirection( player->getOrientation() );
                                // Estado inicial
                                activity = Wait;
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::wayOutTeletransport( PlayerItem * player )
{
        switch ( activity )
        {
                case StartWayOutTeletransport:
                        // Almacena la orientación de salida
                        player->setOrientation( player->getDirection() );
                        // Cambia a las burbujas pero conservando la etiqueta del jugador
                        player->changeItemData( itemDataManager->findItemByLabel( transitionDataLabel ), NoDirection );
                        // Inicia el teletransporte
                        activity = WayOutTeletransport;
                        break;

                case WayOutTeletransport:
                        // Anima el elemento. Si ha llegado al final hay cambio de sala
                        if ( player->animate() )
                        {
                                player->addToZ( -1 );
                                player->setExit( player->getMediator()->collisionWithByLabel( TeleportLabel ) ? ByTeleport : ByTeleportToo );
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::destroy( PlayerItem* player )
{
        switch ( activity )
        {
                // El jugador ha sido destruido por un enemigo
                case StartDestroy:
                        // La destrucción se ejecuta siempre y cuando no tenga inmunidad
                        if ( player->getShieldTime() == 0 )
                        {
                                // Cambia a las burbujas pero conservando la etiqueta del jugador
                                player->changeItemData( itemDataManager->findItemByLabel( transitionDataLabel ), NoDirection );
                                // Inicia la destrucción
                                activity = Destroy;
                        }
                        // Si tiene inmunidad vuelve al estado inicial
                        else
                        {
                                activity = Wait;
                        }
                        break;

                case Destroy:
                        // Anima el elemento. Si ha llegado al final la sala se reinicia
                        if ( player->animate() )
                        {
                                player->setExit( Restart );
                                player->loseLife();
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::useHooter( PlayerItem* player )
{
        // El jugador puede disparar si tiene la bocina y rosquillas
        if ( player->hasTool( Horn ) && player->getAmmo() > 0 )
        {
                this->fireFromHooterIsPresent = true;

                ItemData* hooterData = this->itemDataManager->findItemByLabel( labelOfFireFromHooter );

                if ( hooterData != 0 )
                {
                        // Detiene el sonido del disparo
                        SoundManager::getInstance()->stop( player->getLabel(), Doughnut );

                        // Crea el elemento en la misma posición que el jugador y a su misma altura
                        int z = player->getZ() + player->getHeight() - hooterData->height;
                        FreeItem* freeItem = new FreeItem
                        (
                                hooterData,
                                player->getX(), player->getY(),
                                z < 0 ? 0 : z,
                                player->getDirection()
                        );

                        freeItem->assignBehavior( FireDoughnutBehavior, 0 );
                        FireDoughnut * doughnutBehavior = dynamic_cast< FireDoughnut * >( freeItem->getBehavior() );
                        doughnutBehavior->setPlayerItem( player );

                        // En un primer momento no detecta colisiones ya que parte de la misma posición del jugador
                        freeItem->setCollisionDetector( false );

                        // Se añade a la sala actual
                        player->getMediator()->getRoom()->addItem( freeItem );

                        // Se gasta una rosquillas
                        player->consumeAmmo();

                        // Emite el sonido del disparo
                        SoundManager::getInstance()->play( player->getLabel(), Doughnut );
                }
        }
}

void UserControlled::take( PlayerItem * player )
{
        // El jugador puede coger objetos si tiene el bolso
        if ( player->hasTool( Handbag ) )
        {
                Mediator* mediator = player->getMediator();
                Item* takenItem = 0;

                // Comprueba si hay elementos debajo
                if ( ! player->checkPosition( 0, 0, -1, Add ) )
                {
                        int coordinates = 0;

                        // Selecciona elementos libres empujables de tamaño menor o igual a 3/4 el tamaño de la loseta
                        // De haber varios se seleccionará aquel con las coordenadas espaciales mayores
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                Item* bottomItem = mediator->findCollisionPop( );

                                if ( bottomItem != 0 && bottomItem->getBehavior() != 0 &&
                                        ( bottomItem->getBehavior()->getBehaviorOfItem() == MobileBehavior || bottomItem->getBehavior()->getBehaviorOfItem() == TrampolineBehavior ) &&
                                                bottomItem->getWidthX() <= ( mediator->getTileSize() * 3 ) / 4 &&
                                                bottomItem->getWidthY() <= ( mediator->getTileSize() * 3 ) / 4 )
                                {
                                        if ( bottomItem->getX() + bottomItem->getY() > coordinates )
                                        {
                                                coordinates = bottomItem->getX() + bottomItem->getY();
                                                takenItem = bottomItem;
                                                takenItemBehavior = bottomItem->getBehavior()->getBehaviorOfItem ();
                                        }
                                }
                        }

                        // Guarda el elemento seleccionado y lo hace desaparecer de la sala
                        if ( takenItem != 0 )
                        {
                                takenItemData = itemDataManager->findItemByLabel( takenItem->getLabel() );
                                takenItem->getBehavior()->changeActivityOfItem( Destroy );
                                activity = ( activity == TakeAndJump ? Jump : TakenItem );

                                // Comunica al gestor del juego el elemento que se ha cogido
                                takenItemImage = takenItem->getImage();
                                GameManager::getInstance()->setItemTaken( takenItemImage );
                                // Emite el sonido correspondiente
                                SoundManager::getInstance()->play( player->getLabel(), TakeItem );
                        }
                }
        }

        // Estado inicial si no se ha cogido ningún elemento
        if ( activity != TakenItem && activity != Jump )
        {
                activity = Wait;
        }
}

void UserControlled::drop( PlayerItem* player )
{
        // El jugador debe haber cogido algún elemento
        if ( takenItemData != 0 )
        {
                // El elemento se deja justo debajo. Si el jugador no puede ascender no es posible dejarlo
                if ( player->addToZ( LayerHeight ) )
                {
                        // Crea el elemento en la misma posición pero debajo del jugador
                        FreeItem* freeItem = new FreeItem( takenItemData,
                                                           player->getX(), player->getY(),
                                                           player->getZ() - LayerHeight,
                                                           NoDirection );

                        // Sólo pueden cogerse los elementos portátiles o el trampolín
                        freeItem->assignBehavior( takenItemBehavior, 0 );

                        // Se añade a la sala actual
                        player->getMediator()->getRoom()->addItem( freeItem );

                        // El jugador ya no tiene el elemento
                        takenItemData = 0;
                        takenItemImage = 0;
                        takenItemBehavior = NoBehavior;

                        // Se actualiza el estado para que salte o se detenga
                        activity = ( activity == DropAndJump ? Jump : Wait );

                        // Comunica al gestor del juego que el bolso está vacío
                        GameManager::getInstance()->setItemTaken( takenItemImage );
                        // Emite el sonido correspondiente
                        SoundManager::getInstance()->stop( player->getLabel(), Fall );
                        SoundManager::getInstance()->play( player->getLabel(), DropItem );
                }
                else
                {
                        // Se emite sonido de o~ ou
                        SoundManager::getInstance()->play( player->getLabel(), Mistake );
                }
        }

        // Estado inicial si no se ha dejado ningún elemento
        if ( activity != Jump )
        {
                activity = Wait;
        }
}

void UserControlled::setMoreData( void * data )
{
        this->itemDataManager = reinterpret_cast< ItemDataManager * >( data );
}

void UserControlled::assignTakenItem( ItemData* itemData, BITMAP* takenItemImage, const BehaviorOfItem& behavior )
{
        this->takenItemData = itemData;
        this->takenItemImage = takenItemImage;
        this->takenItemBehavior = behavior;
}

}
