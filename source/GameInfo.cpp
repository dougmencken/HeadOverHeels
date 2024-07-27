
#include "GameInfo.hpp"

#if defined( DEBUG ) && DEBUG
#  include "util.hpp"
#  include <iostream>
#endif


unsigned char GameInfo::getLivesByName ( const std::string & character ) const
{
        if ( character == "headoverheels" )
        {
                return std::min( this->headLives, this->heelsLives );
        }
        else if ( character == "head" )
        {
                return this->headLives ;
        }
        else if ( character == "heels" )
        {
                return this->heelsLives ;
        }

        return 0 ;
}

void GameInfo::addLivesByName ( const std::string & character, unsigned char lives )
{
        unsigned char  newHeadLives = this->headLives  + lives ;
        unsigned char newHeelsLives = this->heelsLives + lives ;

        if (  newHeadLives > 99 ) newHeadLives  = 99 ;
        if ( newHeelsLives > 99 ) newHeelsLives = 99 ;

        if ( character == "head" || character == "headoverheels" )
                setHeadLives( newHeadLives ) ;

        if ( character == "heels" || character == "headoverheels" )
                setHeelsLives( newHeelsLives ) ;
}

void GameInfo::loseLifeByName ( const std::string & character )
{
        unsigned char  newHeadLives = (  this->headLives > 0 ) ? ( this->headLives  - 1 ) : 0 ;
        unsigned char newHeelsLives = ( this->heelsLives > 0 ) ? ( this->heelsLives - 1 ) : 0 ;

        if ( character == "head" || character == "headoverheels" )
                setHeadLives( newHeadLives ) ;

        if ( character == "heels" || character == "headoverheels" )
                setHeelsLives( newHeelsLives ) ;
}

void GameInfo::addQuickStepsByName ( const std::string & character, unsigned short moreBonusQuickSteps )
{
        if ( character == "head" || character == "headoverheels" )
        {
                unsigned short howManyBonusQuickSteps = this->bonusQuickSteps + moreBonusQuickSteps ;
                if ( howManyBonusQuickSteps > 99 ) howManyBonusQuickSteps = 99 ;
                setBonusQuickSteps( howManyBonusQuickSteps ) ;
        }
}

void GameInfo::decrementQuickStepsByName ( const std::string & character )
{
        if ( character == "head" || character == "headoverheels" )
        {
                unsigned short howManyBonusQuickSteps = this->bonusQuickSteps ;
                if ( howManyBonusQuickSteps > 0 )
                        setBonusQuickSteps( howManyBonusQuickSteps - 1 ) ;
        }
}

void GameInfo::addHighJumpsByName ( const std::string & character, unsigned short moreBonusHighJumps )
{
        if ( character == "heels" || character == "headoverheels" )
        {
                unsigned short howManyBonusHighJumps = this->bonusHighJumps + moreBonusHighJumps ;
                if ( howManyBonusHighJumps > 99 ) howManyBonusHighJumps = 99 ;
                setBonusHighJumps( howManyBonusHighJumps ) ;
        }
}

void GameInfo::decrementHighJumpsByName ( const std::string & character )
{
        if ( character == "heels" || character == "headoverheels" )
        {
                unsigned short howManyBonusHighJumps = this->bonusHighJumps ;
                if ( howManyBonusHighJumps > 0 )
                        setBonusHighJumps( howManyBonusHighJumps - 1 ) ;
        }
}

short GameInfo::getShieldPointsByName ( const std::string & character ) const
{
        if ( character == "headoverheels" )
        {
                return std::max( this->headShieldPoints, this->heelsShieldPoints );
        }
        else if ( character == "head" )
        {
                return this->headShieldPoints ;
        }
        else if ( character == "heels" )
        {
                return this->heelsShieldPoints ;
        }

        return 0 ;
}

void GameInfo::setShieldPointsByName ( const std::string & character, short points )
{
        short pointsBefore = getShieldPointsByName( character ) ;
        if ( points == pointsBefore ) return ;

#if defined( DEBUG ) && DEBUG
        if ( points < pointsBefore - 1 ) {
                std::cout << "non-sequential decrement of shield points for character " << character
                                << " : was " << pointsBefore << " and now changes to " << points << std::endl ;
                util::printBacktrace () ;
        }
#endif

        if ( points > 99 ) points = 99 ;
        if ( points <  0 ) points =  0 ;

        if ( character == "head" || character == "headoverheels" )
                setHeadShieldPoints( points ) ;

        if ( character == "heels" || character == "headoverheels" )
                setHeelsShieldPoints( points ) ;
}

void GameInfo::takeMagicTool ( const std::string & tool )
{
        if ( tool == "horn" ) this->horn = true ;
        if ( tool == "handbag" ) this->handbag = true ;
}

void GameInfo::consumeOneDoughnut ()
{
        if ( this->donuts > 0 ) this->donuts -- ;
}

void GameInfo::resetForANewGame ()
{
        this->headLives = 8 ;
        this->heelsLives = 8 ;
        this->bonusQuickSteps = 0 ;
        this->bonusHighJumps = 0 ;
        this->headShieldPoints = 0 ;
        this->heelsShieldPoints = 0 ;
        this->horn = false ;
        this->handbag = false ;
        this->donuts = 0 ;
}
