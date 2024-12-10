
#include "SlideWithHeadAndHeels.hpp"

#include "GameManager.hpp"
#include "GamePreferences.hpp"

#include "AnimatedPictureWidget.hpp"
#include "Label.hpp"
#include "TextField.hpp"

#include "ospaths.hpp"


namespace gui
{

const float delayBetweenFrames = 0.1 ;


SlideWithHeadAndHeels::SlideWithHeadAndHeels( )
        : Slide( )
        , pictureOfHead( nilPointer )
        , pictureOfHeels( nilPointer )
{
        refresh () ;
}

SlideWithHeadAndHeels::~SlideWithHeadAndHeels( )
{
        if ( pictureOfHead != nilPointer && ! pictureOfHead->isOnSomeSlide() ) {
                delete pictureOfHead ;
                this->pictureOfHead = nilPointer ;
        }

        if ( pictureOfHeels != nilPointer && ! pictureOfHeels->isOnSomeSlide() ) {
                delete pictureOfHeels ;
                this->pictureOfHeels = nilPointer ;
        }
}

void SlideWithHeadAndHeels::placeHeadAndHeels( bool picturesToo, bool copyrightsToo )
{
        const unsigned int screenWidth = GamePreferences::getScreenWidth();
        const unsigned int space = ( screenWidth / 20 ) - 10;

        Label* Head = new Label( "Head", new Font( "yellow", true ) );
        Label* over = new Label( "over", new Font( "white" ), /* multicolor */ true );
        Label* Heels = new Label( "Heels", new Font( "yellow", true ) );

        over->moveTo( ( screenWidth - over->getWidth() - 20 ) >> 1, space + Head->getHeight() - over->getHeight() - 8 );
        addWidget( over );

        unsigned int widthOfSpace = over->getFont().getWidthOfLetter( " " );
        int spaceWithoutSpacing = widthOfSpace - over->getSpacing ();

        Head->moveTo( over->getX() - Head->getWidth() - spaceWithoutSpacing, space );
        addWidget( Head );

        Heels->moveTo( over->getX() + over->getWidth() + spaceWithoutSpacing, space );
        addWidget( Heels );

        TextField* JonRitman = new TextField( screenWidth >> 2, "center" ) ;
        JonRitman->appendLine( "Jon", false, "multicolor" );
        JonRitman->appendLine( "Ritman", false, "multicolor" );
        JonRitman->moveTo( Head->getX() - JonRitman->getWidthOfField() - space, space );
        addWidget( JonRitman );

        TextField* BernieDrummond = new TextField( screenWidth >> 2, "center" ) ;
        BernieDrummond->appendLine( "Bernie", false, "multicolor" );
        BernieDrummond->appendLine( "Drummond", false, "multicolor" );
        BernieDrummond->moveTo( Heels->getX() + Heels->getWidth() + space, space );
        addWidget( BernieDrummond );

        if ( picturesToo )
        {
                const unsigned int widthOfSprite = 48 ;
                addPictureOfHeadAt( Head->getX() + ( ( Head->getWidth() - widthOfSprite ) >> 1 ), Head->getY() + Head->getHeight() );
                addPictureOfHeelsAt( Heels->getX() + ( ( Heels->getWidth() - widthOfSprite ) >> 1 ), Heels->getY() + Heels->getHeight() );
        }

        if ( copyrightsToo )
        {
                Label* Jorge = new Label( "{ 2009 Jorge Rodríguez Santos", new Font( "orange" ) );
                Label* Douglas = new Label( "{ 2024 Douglas Mencken", new Font( "yellow" ) );

                const unsigned int screenHeight = GamePreferences::getScreenHeight();
                const int leading = 28;
                const int whereY = screenHeight - space - leading + 4;
                const int whereX = ( screenWidth - Jorge->getWidth() ) >> 1 ;

                // Jorge Rodríguez Santos
                Jorge->moveTo( whereX, whereY );
                addWidget( Jorge );

                // Douglas Mencken
                Douglas->moveTo( whereX, whereY - leading );
                addWidget( Douglas );
        }
}

void SlideWithHeadAndHeels::addPictureOfHeadAt ( int x, int y )
{
        if ( this->pictureOfHead == nilPointer ) {
                const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;
                this->pictureOfHead = new AnimatedPictureWidget(
                                                x, y,
                                                Picture::loadAnimation( ospaths::pathToFile( pathToPictures, "head.gif" ) ),
                                                delayBetweenFrames,
                                                "animated Head" );
        } else
                this->pictureOfHead->moveTo( x, y );

        addWidget( this->pictureOfHead );
}

void SlideWithHeadAndHeels::addPictureOfHeelsAt ( int x, int y )
{
        if ( this->pictureOfHeels == nilPointer ) {
                const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;
                this->pictureOfHeels = new AnimatedPictureWidget(
                                                x, y,
                                                Picture::loadAnimation( ospaths::pathToFile( pathToPictures, "heels.gif" ) ),
                                                delayBetweenFrames,
                                                "animated Heels" );
        } else
                this->pictureOfHeels->moveTo( x, y );

        addWidget( this->pictureOfHeels );
}

void SlideWithHeadAndHeels::refresh ()
{
        Slide::refresh () ;

        if ( this->pictureOfHead != nilPointer )
        {
                int xHead = this->pictureOfHead->getX ();
                int yHead = this->pictureOfHead->getY ();

                if ( this->pictureOfHead->isOnSomeSlide () )
                        removeWidget( this->pictureOfHead );

                delete this->pictureOfHead ;
                this->pictureOfHead = nilPointer ;

                addPictureOfHeadAt( xHead, yHead );
        }

        if ( this->pictureOfHeels != nilPointer )
        {
                int xHeels = this->pictureOfHeels->getX ();
                int yHeels = this->pictureOfHeels->getY ();

                if ( this->pictureOfHeels->isOnSomeSlide () )
                        removeWidget( this->pictureOfHeels );

                delete this->pictureOfHeels ;
                this->pictureOfHeels = nilPointer ;

                addPictureOfHeelsAt( xHeels, yHeels );
        }
}

}
