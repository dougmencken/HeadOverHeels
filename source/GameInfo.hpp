// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GameInfo_hpp_
#define GameInfo_hpp_

#include <string>


/**
 * Holds the game's info for the characters, Head and Heels
 */

class GameInfo
{

public:
        GameInfo( )
                : headLives( 0 )
                , heelsLives( 0 )
                , bonusQuickSteps( 0 )
                , bonusHighJumps( 0 )
                , headShieldPoints( 0 )
                , heelsShieldPoints( 0 )
                , horn( false )
                , handbag( false )
                , donuts( 0 )
        { }

        /**
         * the number of lives every character has
         */

        unsigned char getHeadLives () const {  return this->headLives ;  }

        unsigned char getHeelsLives () const {  return this->heelsLives ;  }

        unsigned char getLivesByName ( const std::string & character ) const ;

        void setHeadLives ( unsigned char lives ) {  this->headLives = lives ;  }

        void setHeelsLives ( unsigned char lives ) {  this->heelsLives = lives ;  }

        void addLivesByName ( const std::string & character, unsigned char lives ) ;

        void loseLifeByName ( const std::string & character ) ;

        /**
         * quick steps and high jumps
         */

        void setBonusQuickSteps ( unsigned short quickSteps ) {  this->bonusQuickSteps = quickSteps ;  }

        unsigned short getBonusQuickSteps () const {  return this->bonusQuickSteps ;  }

        void addQuickStepsByName ( const std::string & character, unsigned short moreBonusQuickSteps ) ;

        void decrementQuickStepsByName ( const std::string & character ) ;

        void setBonusHighJumps ( unsigned short highJumps ) {  this->bonusHighJumps = highJumps ;  }

        unsigned short getBonusHighJumps () const {  return this->bonusHighJumps ;  }

        void addHighJumpsByName ( const std::string & character, unsigned short moreBonusHighJumps ) ;

        void decrementHighJumpsByName ( const std::string & character ) ;

        /**
         * temporary invulnerability
         */

        short getHeadShieldPoints () const {  return this->headShieldPoints ;  }
        short getHeelsShieldPoints () const {  return this->heelsShieldPoints ;  }

        short getShieldPointsByName ( const std::string & character ) const ;

        double getShieldSecondsByName ( const std::string & character ) const
        {
                return GameInfo::convertShieldFromPointsToSeconds( getShieldPointsByName( character ) );
        }

        void setHeadShieldPoints ( short points ) {  this->headShieldPoints = points ;  }
        void setHeelsShieldPoints ( short points ) {  this->heelsShieldPoints = points ;  }

        void setShieldPointsByName ( const std::string & character, short points ) ;

        void setShieldSecondsByName( const std::string & character, double seconds )
        {
                setShieldPointsByName( character, GameInfo::convertShieldFromSecondsToPoints( seconds ) );
        }

        /**
         * the magic tools
         */

        void takeMagicTool ( const std::string & tool ) ;

        bool hasHorn () const {  return this->horn ;  }

        void setHorn ( bool hasHorn ) {  this->horn = hasHorn ;  }

        bool hasHandbag () const {  return this->handbag ;  }

        void setHandbag ( bool hasHandbag ) {  this->handbag = hasHandbag ;  }

        void setDoughnuts ( unsigned short number ) {  this->donuts = number ;  }

        unsigned short getDoughnuts () const {  return this->donuts ;  }

        void consumeOneDoughnut () ;

        void addDoughnuts( unsigned short howMany ) {  setDoughnuts( this->donuts + howMany ) ;  }

        /*
         * various
         */

        void resetForANewGame () ;

private:

        unsigned char headLives ;

        unsigned char heelsLives ;

        // the number (between 0 and 99) of remaining bonus steps of moving at double speed for Head
         /* 1. If the character goes step by step, it does not spend
          * 2. If the character moves but collides, it does not spend
          * 3. Consumes a unit of time every 8 steps, as long as 4 steps are taken in a row
          * 4. When the character jumps it does not consume
          */
        unsigned short bonusQuickSteps ;

        // the number (between 0 and 10) of remaining bonus high jumps for Heels
        unsigned short bonusHighJumps ;

        // the time remaining when Head is inviolable, in "points" 0..99
        short headShieldPoints ;

        // the time remaining when Heels is inviolable, in "points" 0..99
        short heelsShieldPoints ;

        // does Head have the doughnut horn
        bool horn ;

        // does Heels have the hand bag
        bool handbag ;

        // the number of donuts collected by Head
        unsigned short donuts ;

public:

        // a character gets 25 seconds of immunity when a rabbit is touched
        #define _fullShieldTimeInSeconds        25.0

#ifdef __Cxx11__ /* when complier supports c++11 */
        static constexpr double fullShieldTimeInSeconds = _fullShieldTimeInSeconds ;
#else
        #define fullShieldTimeInSeconds         _fullShieldTimeInSeconds
#endif

private:
        // 99 of "points" are equal to fullShieldTimeInSeconds (25) of seconds
        //
        static short convertShieldFromSecondsToPoints( double seconds )
        {
                return static_cast< short >( seconds * 99.0 / fullShieldTimeInSeconds ) ;
        }

        static double convertShieldFromPointsToSeconds( short points )
        {
                return static_cast< double >( points ) * fullShieldTimeInSeconds / 99.0 ;
        }

} ;

#endif
