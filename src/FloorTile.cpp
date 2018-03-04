
#include "FloorTile.hpp"
#include "Mediator.hpp"


namespace isomot
{

FloorTile::FloorTile( int column, int cellX, int cellY, BITMAP* image )
        : Mediated ()
        , column( column )
        , rawImage( image )
        , shadyImage( nilPointer )
{
        this->coordinates.first = cellX;
        this->coordinates.second = cellY;
        this->offset.first = this->offset.second = 0;
        this->shady = NoShadow;
}

FloorTile::~FloorTile()
{
        destroy_bitmap( rawImage );
        destroy_bitmap( shadyImage );
}

void FloorTile::calculateOffset()
{
        if ( mediator != nilPointer )
        {
                Room* room = mediator->getRoom();
                this->offset.first = room->getX0() + ( ( room->getSizeOfOneTile() * ( coordinates.first - coordinates.second - 1 ) ) << 1 ) + 1;
                this->offset.second = room->getY0() + room->getSizeOfOneTile() * ( coordinates.first + coordinates.second );
        }
}

void FloorTile::draw( BITMAP* where )
{
        if ( shadyImage != nilPointer )
        {       // draw tile with shadow
                draw_sprite( where, shadyImage, offset.first, offset.second );
        }
        else if ( rawImage != nilPointer )
        {       // draw tile, just tile
                draw_sprite( where, rawImage, offset.first, offset.second );
        }
}

void FloorTile::requestShadow()
{
        if ( this->rawImage && this->shady == WantShadow )
        {
                mediator->castShadowOnFloor( this );

                // Si no se ha podido sombrear entonces se destruye la imagen de sombreado
                if ( this->shady != AlreadyShady && this->shadyImage )
                {
                        destroy_bitmap( this->shadyImage );
                        this->shadyImage = nilPointer;
                }

                // Reinicia el atributo para el siguiente ciclo
                this->shady = NoShadow;
        }
}

void FloorTile::setShadyImage( BITMAP* newShady )
{
        if ( shadyImage != newShady )
        {
                destroy_bitmap( shadyImage );
                shadyImage = newShady;
        }
}

}
