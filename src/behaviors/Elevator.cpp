
#include "Elevator.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Elevator::Elevator( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        speedTimer = new Timer();
        stopTimer = new Timer();
        speedTimer->go ();
        stopTimer->go ();
}

Elevator::~Elevator( )
{
        delete speedTimer;
        delete stopTimer;
}

bool Elevator::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );

        switch ( activity )
        {
                case Wait:
                        changeActivityOfItem ( ascent ? MoveUp : MoveDown );
                        lastActivity = activity;
                        break;

                case MoveUp:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                MoveKindOfActivity::getInstance()->move( this, &activity, false );

                                speedTimer->reset();

                                // elevator reached its top
                                if ( freeItem->getZ() > top * LayerHeight )
                                {
                                        activity = StopTop;
                                        lastActivity = activity;
                                        stopTimer->reset();
                                }
                        }

                        freeItem->animate();
                        break;

                case MoveDown:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                MoveKindOfActivity::getInstance()->move( this, &activity, false );

                                speedTimer->reset();

                                // elevator reached its bottom
                                if ( freeItem->getZ() <= bottom * LayerHeight )
                                {
                                        activity = StopBottom;
                                        lastActivity = activity;
                                        stopTimer->reset();
                                }
                        }

                        freeItem->animate();
                        break;

                // stop elevator for a moment when it reaches minimum height
                case StopBottom:
                        if ( stopTimer->getValue() >= 0.250 )
                        {
                                changeActivityOfItem( MoveUp );
                                lastActivity = activity;
                        }

                        freeItem->animate();
                        break;

                // stop elevator for a moment when it reaches maximum height
                case StopTop:
                        if ( stopTimer->getValue() >= 0.250 )
                        {
                                changeActivityOfItem( MoveDown );
                                lastActivity = activity;
                        }

                        freeItem->animate();
                        break;

                default:
                        activity = lastActivity;
                        break;
        }

        SoundManager::getInstance()->play( freeItem->getLabel(), activity );

        return false;
}

void Elevator::setMoreData( void * data )
{
        int * values = reinterpret_cast< int * >( data );

        this->top = values[ 0 ];
        this->bottom = values[ 1 ];
        this->ascent = bool( values[ 2 ] );
}

}
