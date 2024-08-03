
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

void GameInfo::updateShield ()
{
        const std::string & activeDude = GameMap::getInstance().getActiveRoom()->getMediator()->getNameOfActiveCharacter() ;

        if ( ! activeDude.empty() ) {
                double oldSeconds = getShieldSecondsByName( activeDude ) ;
                if ( oldSeconds > 0.0 ) {
                        double newSeconds = oldSeconds - this->shieldDecreaseTimer.getValue() ;
                        setShieldSecondsByName( activeDude, newSeconds );
                }
        }

        resetShieldDecreaseTimer() ; // restart the shield-decreasing chronometer
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
        this->headShieldSeconds = 0.0 ;
        this->heelsShieldSeconds = 0.0 ;
        this->horn = false ;
        this->handbag = false ;
        this->donuts = 0 ;
}
