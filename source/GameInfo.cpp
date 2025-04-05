
#include "GameInfo.hpp"

#include "GameMap.hpp"
#include "Mediator.hpp"


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

void GameInfo::addBonusQuickStepsByName ( const std::string & character, unsigned short moreQuickSteps )
{
        if ( character == "head" || character == "headoverheels" )
        {
                unsigned short howManyBonusQuickSteps = this->bonusQuickSteps + moreQuickSteps ;
                if ( howManyBonusQuickSteps > 99 ) howManyBonusQuickSteps = 99 ;
                setBonusQuickSteps( howManyBonusQuickSteps ) ;
        }
}

void GameInfo::decrementBonusQuickStepsByName ( const std::string & character )
{
        if ( character == "head" || character == "headoverheels" )
        {
                unsigned short howManyBonusQuickSteps = this->bonusQuickSteps ;
                if ( howManyBonusQuickSteps > 0 )
                        setBonusQuickSteps( howManyBonusQuickSteps - 1 ) ;
        }
}

void GameInfo::addBonusBigJumpsByName ( const std::string & character, unsigned short moreBonusJumps )
{
        if ( character == "heels" || character == "headoverheels" )
        {
                unsigned short howManyBonusBigJumps = this->bonusBigJumps + moreBonusJumps ;
                if ( howManyBonusBigJumps > 99 ) howManyBonusBigJumps = 99 ;
                setBonusBigJumps( howManyBonusBigJumps ) ;
        }
}

void GameInfo::decrementBonusBigJumpsByName ( const std::string & character )
{
        if ( character == "heels" || character == "headoverheels" )
        {
                unsigned short howManyBonusBigJumps = this->bonusBigJumps ;
                if ( howManyBonusBigJumps > 0 )
                        setBonusBigJumps( howManyBonusBigJumps - 1 ) ;
        }
}

double GameInfo::getShieldSecondsByName ( const std::string & character ) const
{
        if ( character == "headoverheels" )
        {
                return std::max( this->headShieldSeconds, this->heelsShieldSeconds );
        }
        else if ( character == "head" )
        {
                return this->headShieldSeconds ;
        }
        else if ( character == "heels" )
        {
                return this->heelsShieldSeconds ;
        }

        return 0.0 ;
}

void GameInfo::setShieldSecondsByName ( const std::string & character, double seconds )
{
        if ( seconds > fullShieldTimeInSeconds ) seconds = fullShieldTimeInSeconds ;
        if ( seconds < 0.0 ) seconds = 0.0 ;

        if ( character == "head" || character == "headoverheels" )
                setHeadShieldSeconds( seconds ) ;

        if ( character == "heels" || character == "headoverheels" )
                setHeelsShieldSeconds( seconds ) ;
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
        this->bonusBigJumps = 0 ;
        this->headShieldSeconds = 0.0 ;
        this->heelsShieldSeconds = 0.0 ;
        this->horn = false ;
        this->handbag = false ;
        this->donuts = 0 ;
}
