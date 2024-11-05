
#include "AllCreatedActions.hpp"
#include "Action.hpp"

#include "util.hpp" // nilPointer, DeleteIt, IF_DEBUG

#include <algorithm> // std::for_each
#include <iostream>


namespace gui
{

AllCreatedActions AllCreatedActions::instance ;


AllCreatedActions::~AllCreatedActions ()
{
        std::for_each( this->actions.begin(), this->actions.end(), DeleteIt() );
}

/* static */
void AllCreatedActions::add ( Action * newAction )
{
        //*/* AllCreatedActions::getInstance().binFinishedActions() ; */*//

        AllCreatedActions::getInstance().addAction( newAction );

        IF_DEBUG ( AllCreatedActions::getInstance().dumpActions() )
}

void AllCreatedActions::addAction ( Action * actionToAdd )
{
        if ( actionToAdd != nilPointer )
                this->actions.push_back( actionToAdd ) ;
}

void AllCreatedActions::binFinishedActions ()
{
        for ( std::vector< Action * >::iterator it = this->actions.begin() ; it != this->actions.end() ; /* don’t increment here */ ) {
                if ( ( *it )->isDone() )
                        it = this->actions.erase( it );
                else
                        ++ it ;
        }
}

void AllCreatedActions::dumpActions () const
{
        std::cout << "the collection has " ;
        const unsigned int howMany = this->actions.size() ;
        std::cout << howMany << " action" << ( howMany != 1 ? "s " : " " ) << "yet {" ;
        for ( unsigned int a = 0 ; a < howMany ; ++ a ) {
                Action * action = this->actions[ a ] ;
                if ( action == nilPointer ) continue ;
                std::cout << " " << action->getNameOfAction() ;
                if ( action->hasBegun() || action->isDone() )
                        std::cout << ( action->isDone() ? "(done)" : "(doing)" );
                if ( a + 1 < howMany ) std::cout << "," ;
        }
        std::cout << " }" << std::endl ;
}

}
