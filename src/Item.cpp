
#include "Item.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"
#include "Behavior.hpp"

#include <iostream>


namespace isomot
{

Item::Item( ItemData* itemData, int z, const Direction& direction )
: Mediated(),
        id( 0 ),
        label( itemData->label ),
        dataOfItem( itemData ),
        z( z ),
        direction( direction ),
        frameIndex( 0 ),
        backwardMotion( false ),
        myShady( WantShadow ),
        image( 0 ),
        shadow( 0 ),
        processedImage( 0 ),
        behavior( 0 )
{
        this->offset.first = this->offset.second = 0;

        // Si el elemento tiene más de un fotograma por dirección entonces tiene animación
        if ( ( itemData->motion.size() - itemData->extraFrames ) / itemData->directionFrames > 1 )
        {
                motionTimer.start();
        }
}

Item::Item( const Item& item )
: Mediated( item ),
        id( item.id ),
        label( item.label ),
        dataOfItem( item.dataOfItem ),
        x( item.x ),
        y( item.y ),
        z( item.z ),
        direction( item.direction ),
        frameIndex( item.frameIndex ),
        backwardMotion( item.backwardMotion ),
        myShady( item.myShady ),
        image( item.image ),
        shadow( item.shadow ),
        processedImage( 0 ),
        offset( item.offset ),
        motionTimer( item.motionTimer ),
        behavior( 0 )
{
        if ( item.processedImage != 0 )
        {
                this->processedImage = create_bitmap_ex( 32, item.processedImage->w, item.processedImage->h );
                blit( item.processedImage, this->processedImage, 0, 0, 0, 0, this->processedImage->w, this->processedImage->h );
        }
}

Item::~Item( )
{
        if ( this->behavior != 0 )
                delete this->behavior;

        if ( this->processedImage != 0 )
                destroy_bitmap( this->processedImage );
}

bool Item::update()
{
        bool destroy = false;

        if ( behavior != 0 )
                destroy = behavior->update();

        return destroy;
}

bool Item::animate()
{
        bool cycle = false;

        // Si el elemento tiene más de un fotograma por dirección entonces se puede animar
        if ( ( dataOfItem->motion.size() - dataOfItem->extraFrames ) / dataOfItem->directionFrames > 1 )
        {
                // Si ha pasado el tiempo necesario para cambiar de fotograma
                if ( motionTimer.getValue() > dataOfItem->framesDelay )
                {
                        // Reproducción normal, hacia adelante
                        if ( ! backwardMotion )
                        {
                                // Incrementa el índice o vuelve al principio si se alcanzó el final
                                if ( ++frameIndex >= dataOfItem->frames.size() )
                                {
                                        frameIndex = 0;
                                        cycle = true;
                                }
                        }
                        // Reproducción hacia atrás
                        else
                        {
                                // Decrementa el índice o vuelve al final si se alcanzó el principio
                                if ( frameIndex-- <= 0 )
                                {
                                        frameIndex = dataOfItem->frames.size() - 1;
                                        cycle = true;
                                }
                        }

                        // Calcula qué fotograma se tiene que mostrar
                        int framesNumber = ( dataOfItem->motion.size() - dataOfItem->extraFrames ) / dataOfItem->directionFrames;
                        int currentFrame = dataOfItem->frames[ frameIndex ] + ( dataOfItem->directionFrames > 1 ? framesNumber * direction : 0 );

                        // Si la imagen actual es distinta a la nueva imagen entonces se cambia
                        if ( this->image != 0 && this->image != dataOfItem->motion[ currentFrame ] )
                        {
                                changeImage( dataOfItem->motion[ currentFrame ] );

                                if ( this->shadow != 0 )
                                {
                                        changeShadow( dataOfItem->shadows[ currentFrame ] );
                                }
                        }

                        // Reinicia el cronómetro
                        motionTimer.reset();
                }
        }

        return cycle;
}

void Item::changeItemData( ItemData* itemData, const Direction& direction )
{
        // Cambia los datos del elemento excepto la etiqueta. De lo contrario se
        // identificaría como un elemento totalmente distinto
        this->dataOfItem = itemData;
        this->direction = direction;
        this->frameIndex = 0;
        this->backwardMotion = false;
}

void Item::changeDirection( const Direction& direction )
{
        // Se cambia la dirección sólo si el elemento tiene distintos fotogramas para representar
        // distintas direcciones
        if ( dataOfItem->directionFrames > 1 )
        {
                // Cálculo del fotograma correspondiente a la nueva dirección
                unsigned int currentFrame = ( ( dataOfItem->motion.size() - dataOfItem->extraFrames ) / dataOfItem->directionFrames ) * direction;

                // Si la imagen actual es distinta a la nueva imagen entonces se cambia
                if ( this->image != 0 && currentFrame < dataOfItem->motion.size() && this->image != dataOfItem->motion[ currentFrame ] )
                {
                        // Actualiza la dirección
                        this->direction = direction;

                        // Actualiza la imagen
                        changeImage( dataOfItem->motion[ currentFrame ] );

                        // Si cambia la imagen entonces cambia la sombra
                        if ( this->shadow != 0 )
                                changeShadow( dataOfItem->shadows[ currentFrame ] );
                }
        }
}

void Item::changeFrame( const unsigned int frameIndex )
{
        // Si el índice especificado está dentro de los límites del vector que almacena la secuencia de
        // animación, entonces se puede hacer el cambio
        if ( dataOfItem->motion.size() > frameIndex )
        {
                this->frameIndex = frameIndex;

                // Si la imagen actual es distinta a la nueva imagen entonces se cambia
                if ( this->image != 0 && this->image != dataOfItem->motion[ frameIndex ] )
                {
                        changeImage( dataOfItem->motion[ frameIndex ] );

                        // Si cambia la imagen entonces cambia la sombra
                        if ( this->shadow != 0 )
                                changeShadow( dataOfItem->shadows[ frameIndex ] );
                }
        }
}

bool Item::checkPosition( int x, int y, int z, const WhatToDo& what )
{
        bool collisionFound = false;
        // Coordenadas del elemento antes del cambio
        int px = this->x;
        int py = this->y;
        int pz = this->z;

        // Vacía la pila de colisiones
        mediator->clearStackOfCollisions( );

        // Nuevas coordenadas
        this->x = x + this->x * what;
        this->y = y + this->y * what;
        this->z = z + this->z * what;

        // Se buscan colisiones con las paredes reales, aquellas que delimitan la sala
        if ( this->x < mediator->getBound( North ) )
        {
                mediator->pushCollision( NorthWall );
        }
        else if ( this->x + this->dataOfItem->widthX > mediator->getBound( South ) )
        {
                mediator->pushCollision( SouthWall );
        }
        if ( this->y >= mediator->getBound( West ) )
        {
                mediator->pushCollision( WestWall );
        }
        else if ( this->y - this->dataOfItem->widthY + 1 < mediator->getBound( East ) )
        {
                mediator->pushCollision( EastWall );
        }

        // Se buscan colisiones con el suelo
        if ( this->z < 0 )
        {
                mediator->pushCollision( Floor );
        }

        // Si ha habido colisión con algún elemento especial se detiene el proceso
        if ( ! ( collisionFound = ! mediator->isStackOfCollisionsEmpty () ) )
        {
                // Busca colisiones con el resto de elementos de la sala
                collisionFound = mediator->findCollisionWithItem( this );
        }

        // Restaura las coordenadas originales
        this->x = px;
        this->y = py;
        this->z = pz;

        return ! collisionFound;
}

void Item::assignBehavior( const BehaviorId& id, void* extraData )
{
        // Construye el objeto adecuado en función del identificador
        this->behavior = Behavior::createBehavior( this, id, extraData );
}

void Item::setForwardMotion()
{
        this->backwardMotion = false;
        this->frameIndex = 0;
}

void Item::setBackwardMotion()
{
        this->backwardMotion = true;
        this->frameIndex = dataOfItem->frames.size() - 1;
}

int Item::getWidthX() const
{
        return dataOfItem->widthX;
}

int Item::getWidthY() const
{
        return dataOfItem->widthY;
}

void Item::setHeight( int height )
{
        dataOfItem->height = height;
}

int Item::getHeight() const
{
        return dataOfItem->height;
}

bool Item::isMortal() const
{
        return dataOfItem->mortal;
}

unsigned char Item::getDirectionFrames() const
{
        return dataOfItem->directionFrames;
}

double Item::getSpeed() const
{
        return dataOfItem->speed;
}

double Item::getWeight() const
{
        return dataOfItem->weight;
}

double Item::getFramesDelay() const
{
        return dataOfItem->framesDelay;
}

unsigned int Item::countFrames() const
{
        return dataOfItem->motion.size();
}

void Item::setAnchor( Item* item )
{
        this->anchor = item;
}

}
