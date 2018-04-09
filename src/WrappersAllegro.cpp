
#include "WrappersAllegro.hpp"

#include <string>
#include <iostream>


namespace allegro
{

bool initialized = false ;

void init( /*~ const std::string& printMe = "" ~*/ )
{
        /*~ if ( ! printMe.empty() ) std::cout << "( allegro::init ) " << printMe << std::endl; ~*/

        allegro_init();

        initialized = true;
}

void drawSprite( BITMAP* view, BITMAP* sprite, int x, int y /*~ const std::string& printMe = "" ~*/ )
{
        if ( ! initialized ) {  std::cerr << "allegro::drawSprite before allegro::init" << std::endl ; return ;  }

        if ( sprite == NULL ) return ;

        /*~ if ( ! printMe.empty() ) std::cout << "( allegro::drawSprite ) " << printMe << std::endl; ~*/

        draw_sprite( view, sprite, x, y );
}

void destroyBitmap( BITMAP*& bitmap /*~ , const std::string& printMe = "" ~*/ )
{
        if ( ! initialized ) {  std::cerr << "allegro::destroyBitmap before allegro::init" << std::endl ; return ;  }

        if ( bitmap == NULL ) return ;

        /*~ if ( ! printMe.empty() ) std::cout << "( allegro::destroyBitmap ) " << printMe << std::endl; ~*/

        // nullify it first, then invoke destroy_bitmap
        // to avoid drawing of being-destroyed thing when there’re many threads

        BITMAP* toBin = bitmap;
        bitmap = NULL ;

        destroy_bitmap( toBin );
}

}
