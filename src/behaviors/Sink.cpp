
#include "Sink.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FallKindOfActivity.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Sink::Sink( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        fallTimer = new Timer();
        fallTimer->go();
}

Sink::~Sink()
{
        delete fallTimer;
}

bool Sink::update ()
{
        GridItem * gridItem = dynamic_cast< GridItem * >( this->item );

        switch ( activity )
        {
                case Wait:
                        // Si tiene un elemento encima entonces el elemento empieza a descender
                        if ( ! gridItem->checkPosition( 0, 0, 1, Add ) )
                        {
                                this->changeActivityOfItem( Fall );
                        }
                        break;

                case Fall:
                        // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                        if ( fallTimer->getValue() > gridItem->getWeight() )
                        {
                                // Si no puede seguir descendiendo o ya no hay ningún elemento encima entonces se detiene
                                if ( ! whatToDo->fall( this ) || gridItem->checkPosition( 0, 0, 1, Add ) )
                                {
                                        activity = Wait;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                fallTimer->reset();
                        }
                        break;

                default:
                        activity = Wait;
        }

        // Emite el sonido acorde al estado
        SoundManager::getInstance()->play( gridItem->getLabel(), activity );

        return false;
}

}
