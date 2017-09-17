
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
        speedTimer = new HPC();
        stopTimer = new HPC();
        speedTimer->start();
        stopTimer->start();
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
                                whatToDo->move( this, &activity, false );

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();

                                // Si ha llegado el techo entonces empieza el descenso
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
                                // El elemento se mueve
                                whatToDo->move( this, &activity, false );

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();

                                // Si ha llegado al suelo entonces empieza el ascenso
                                if ( freeItem->getZ() <= bottom * LayerHeight )
                                {
                                        activity = StopBottom;
                                        lastActivity = activity;
                                        stopTimer->reset();
                                }
                        }

                        freeItem->animate();
                        break;

                // Detiene el ascensor un instante cuando alcanza la altura mínima
                case StopBottom:
                        if ( stopTimer->getValue() >= 0.250 )
                        {
                                changeActivityOfItem( MoveUp );
                                lastActivity = activity;
                        }

                        freeItem->animate();
                        break;

                // Detiene el ascensor un instante cuando alcanza la altura máxima
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
                        whatToDo = MoveKindOfActivity::getInstance();
        }

        // Emite el sonido correspondiente
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
