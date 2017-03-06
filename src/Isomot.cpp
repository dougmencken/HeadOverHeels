
#include "Isomot.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "ItemDataManager.hpp"
#include "MapManager.hpp"
#include "BonusManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "PlayerItem.hpp"
#include "Behavior.hpp"

namespace isomot
{

Isomot::Isomot()
: isEndRoom( false )
{
        // Crea la imagen donde se dibujará la vista isométrica
        this->view = create_bitmap_ex( 32, ScreenWidth, ScreenHeight );

        // Se cargan y almacenan los datos de los elementos del juego
        this->itemDataManager = new ItemDataManager( "items.xml" );
        this->itemDataManager->load();

        // Se crea y se pone en marcha el gestor del mapa
        this->mapManager = new MapManager( this, "map.xml" );
        this->mapManager->load();
}

Isomot::~Isomot()
{
        delete this->mapManager;
        delete this->itemDataManager;
        destroy_bitmap( this->view );
}

void Isomot::beginNew()
{
        this->prepare ();

        // Se crean las salas iniciales
        mapManager->beginNewGame( "blacktooth/blacktooth01.xml", "blacktooth/blacktooth23.xml" );
        //mapManager->beginNewGame( "blacktooth/blacktooth85.xml", "blacktooth/blacktooth23.xml" );

        // Pone en marcha la sala inicial
        mapManager->getActiveRoom()->startUpdate ();

        std::cout << "play new game" << std::endl ;
        SoundManager::getInstance()->playOgg ( "music/begin.ogg", /* loop */ false );
}

void Isomot::beginOld( const sgxml::players::player_sequence& playerSequence )
{
        this->prepare ();

        // Se crean las salas iniciales
        for ( sgxml::players::player_const_iterator i = playerSequence.begin (); i != playerSequence.end (); ++i )
        {
                this->mapManager->beginOldGameWithPlayer( *i );
        }

        std::cout << "continue previous game" << std::endl ;
        // no begin.ogg here
}

void Isomot::prepare ()
{
        this->isEndRoom = false;

        // Crea la imagen donde se dibujará la vista isométrica
        if ( this->view == 0 )
        {
                this->view = create_bitmap_ex( 32, ScreenWidth, ScreenHeight );
        }

        // Se cargan y almacenan los datos de los elementos del juego
        if ( this->itemDataManager == 0 )
        {
                this->itemDataManager = new ItemDataManager( "items.xml" );
                this->itemDataManager->load ();
        }

        // Se crea y se pone en marcha el gestor del mapa
        if ( this->mapManager == 0 )
        {
                this->mapManager = new MapManager( this, "map.xml" );
                this->mapManager->load ();
        }

        if ( GameManager::getInstance()->areLivesInexhaustible () )
        {
                GameManager::getInstance()->toggleInfiniteLives ();
        }
}

void Isomot::pause ()
{
        // Detiene la actualización de los estados de los elementos y
        // la representación de la vista isométrica
        if( mapManager->getActiveRoom() != 0 )
        {
                mapManager->getActiveRoom()->deactivate();
        }
}

void Isomot::resume ()
{
        // Reactiva la actualización de los estados de los elementos y
        // la representación de la vista isométrica
        if( mapManager->getActiveRoom() != 0 )
        {
                mapManager->getActiveRoom()->activate();
        }
}

void Isomot::reset()
{
        this->isEndRoom = false;

        // Destruye la vista isométrica actual
        if( this->view != 0 )
        {
                destroy_bitmap( this->view );
                this->view = 0;
        }

        // El gestor del mapa debe limpiar todos sus datos a excepción del propio mapa
        this->mapManager->reset();

        // Elimina los bonus marcados como ausentes en todas las salas
        BonusManager::getInstance()->reset();
}

BITMAP* Isomot::update()
{
        Room* activeRoom = mapManager->getActiveRoom();
        GameManager* gameManager = GameManager::getInstance();

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_F ] )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                key[ KEY_F ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_I ] )
        {
                // Activa o desactiva las vidas infinitas
                gameManager->toggleInfiniteLives ();
                key[ KEY_I ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_B ] )
        {
                gameManager->toggleBackgroundPicture ();
                key[ KEY_B ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_S ] )
        {
                gameManager->eatFish ( activeRoom->getMediator()->getActivePlayer(), activeRoom );
                key[ KEY_S ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_8_PAD ] )
        {
                activeRoom->getCamera()->setDeltaY( activeRoom->getCamera()->getDeltaY() - 2 );
        }
        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_2_PAD ] )
        {
                activeRoom->getCamera()->setDeltaY( activeRoom->getCamera()->getDeltaY() + 2 );
        }
        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_4_PAD ] )
        {
                activeRoom->getCamera()->setDeltaX( activeRoom->getCamera()->getDeltaX() - 2 );
        }
        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_6_PAD ] )
        {
                activeRoom->getCamera()->setDeltaX( activeRoom->getCamera()->getDeltaX() + 2 );
        }

        // Si se pulsa la tecla de intercambio se cambia de personaje y/o de sala
        if ( ! this->isEndRoom && InputManager::getInstance()->swap() )
        {
                // Antes de cambiar el jugador se detiene el actual
                activeRoom->getMediator()->getActivePlayer()->wait();
                if ( activeRoom->getMediator()->getActivePlayer()->getBehavior()->getStateId() == StateWait )
                {
                        // Si se puede cambiar de jugador en la misma sala hay que comunicar
                        // al gestor del mapa este hecho
                        if ( activeRoom->changePlayer( this->itemDataManager ) )
                        {
                                mapManager->updateActivePlayer();
                        }
                        // En caso contrario habrá que cambiar de sala
                        else
                        {
                                activeRoom = mapManager->swapRoom();
                        }
                }
                // Las repeticiones de esta tecla no deben procesarse
                InputManager::getInstance()->noRepeat( "swap" );
        }

        // Si la sala está activa entonces se dibuja
        if ( activeRoom->isActive() )
        {
                activeRoom->draw();
        }
        // En caso contrario hay un cambio de sala o el jugador activo ha perdido una vida
        else
        {
                Direction exit = activeRoom->getExit();

                if ( exit == Restart )
                {
                        PlayerItem* player = activeRoom->getMediator()->getActivePlayer();

                        if ( player->getLives() != 0 || ( PlayerId(player->getLabel ()) == HeadAndHeels && player->getLives() == 0 ) )
                        {
                                activeRoom = mapManager->restartRoom();
                        }
                        else
                        {
                                // Si se puede cambiar de jugador en la misma sala hay que comunicar
                                // al gestor del mapa este hecho
                                if( activeRoom->alivePlayer( this->itemDataManager ) )
                                {
                                        mapManager->updateActivePlayer();
                                }
                                // En caso contrario habrá que cambiar de sala
                                else
                                {
                                        activeRoom = mapManager->destroyAndSwapRoom();
                                }
                        }
                }
                else
                {
                        activeRoom = mapManager->changeRoom( exit );

                        std::string scenery = activeRoom->getScenery ();
                        SoundManager::getInstance()->playOgg ( "music/" + scenery + ".ogg", /* loop */ false );
                }
        }

        // Si hay una sala activa, se dibuja
        if ( activeRoom != 0 )
        {
                blit (
                        activeRoom->getPicture(), this->view,
                        activeRoom->getCamera()->getDeltaX(), activeRoom->getCamera()->getDeltaY(),
                        0, 0,
                        activeRoom->getPicture()->w, activeRoom->getPicture()->h
                );

                // Delata al tramposo
                if ( GameManager::getInstance()->areLivesInexhaustible () )
                {
                        textout_ex( this->view, font, "VIDAS INFINITAS", 18, 10, makecol( 255, 255, 255 ), -1 );
                        textout_ex( this->view, font, "INFINITE LIVES", this->view->w - 128, 10, makecol( 255, 255, 255 ), -1 );
                }

                // La sala final es muy especial
                if ( activeRoom->getIdentifier().compare( "blacktooth/blacktooth88.xml" ) == 0 )
                {
                        this->updateEndRoom();
                }
        }
        // Si no hay sala activa es que la partida ha terminado
        else
        {
                destroy_bitmap( this->view );
                this->view = 0;
        }

        return this->view;
}

void Isomot::updateEndRoom()
{
        Room* activeRoom = mapManager->getActiveRoom();

        // Si aún no se ha preparado la sala final, se hace ahora
        if( ! this->isEndRoom )
        {
                short player = activeRoom->getMediator()->getActivePlayer()->getLabel();
                activeRoom->getMediator()->removeItem( activeRoom->getMediator()->getActivePlayer() );

                // Crea (como elemento libre) al jugador que ha llegado en la posición adecuada
                FreeItem* freeItem = new FreeItem( this->itemDataManager->find( player ), 66, 92, Top, South );
                activeRoom->addItem( freeItem );

                // Crea las coronas recuperadas
                GameManager* gameManager = GameManager::getInstance();
                int crowns = 0;

                // La corona de Safari
                if( gameManager->isFreePlanet( Safari ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->find( short(Crown) ), 66, 75, Top, NoDirection );
                        activeRoom->addItem( freeItem );
                        crowns++;
                }
                // La corona de Egyptus
                if( gameManager->isFreePlanet( Egyptus ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->find( short(Crown) ), 66, 59, Top, NoDirection );
                        activeRoom->addItem( freeItem );
                        crowns++;
                }
                // La corona de Penitentiary
                if( gameManager->isFreePlanet( Penitentiary ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->find( short(Crown) ), 65, 107, Top, NoDirection );
                        activeRoom->addItem( freeItem );
                        crowns++;
                }
                // La corona de Byblos
                if( gameManager->isFreePlanet( Byblos ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->find( short(Crown) ), 65, 123, Top, NoDirection );
                        activeRoom->addItem( freeItem );
                        crowns++;
                }
                // La corona de Blacktooth
                if( gameManager->isFreePlanet( Blacktooth ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->find( short(Crown) ), 65, 91, Top, NoDirection );
                        activeRoom->addItem( freeItem );
                        crowns++;
                }

                // Si se han conseguido las cinco coronas se mostrará la pantalla de felicitación
                if( crowns == 5 )
                {
                        gameManager->success();
                }
                // Si no se saldrá directamente a la pantalla resumen
                else
                {
                        gameManager->arriveFreedom();
                }

                // Ahora la sala ya está lista
                this->isEndRoom = true;
        }
        else
        {
                // Se dispara una salva de honor
                if( activeRoom->getMediator()->findItemByLabel( CannonBallLabel ) == 0 && activeRoom->getMediator()->findItemByLabel( BubblesLabel ) == 0 )
                {
                        FreeItem* freeItem = new FreeItem( this->itemDataManager->find( CannonBallLabel ), 146, 93, LayerHeight, NoDirection );
                        freeItem->assignBehavior( CannonBallBehavior, this->itemDataManager->find( BubblesLabel ) );
                        activeRoom->addItem( freeItem );
                }
        }
}

ItemDataManager* Isomot::getItemDataManager() const
{
        return itemDataManager;
}

MapManager* Isomot::getMapManager() const
{
        return this->mapManager;
}

}
