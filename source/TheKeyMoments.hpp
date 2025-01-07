// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TheKeyMoments_hpp_
#define TheKeyMoments_hpp_

/**
 * Marks the definitive moments of the game
 *
 * Only the single l’one of these moments can be set as true at the same time
 */

class TheKeyMoments
{

private:

        bool theMomentOfGameOver ;   /* all lives are lost or the player quit the game */

        bool theMomentOfEatenFish ;        /* a chance to save the current progress */

        bool theMomentOfCrownWasTaken ;    /* a crown is taken and one more planet was liberated this way */

        bool theMomentOfArrivalInFreedomNotWithAllCrowns ;            /* at least one character reached Freedom (not with all crowns) */
        bool theMomentWhenHeadAndHeelsAreInFreedomWithAllTheCrowns ;  /* both characters reached Freedom with all the crowns */

        // no copy constructor
        TheKeyMoments( const TheKeyMoments & ) ;

public:

        TheKeyMoments()
                : theMomentOfGameOver( false )
                , theMomentOfEatenFish( false )
                , theMomentOfCrownWasTaken( false )
                , theMomentOfArrivalInFreedomNotWithAllCrowns( false )
                , theMomentWhenHeadAndHeelsAreInFreedomWithAllTheCrowns( false )
        {}

        // the player quit the game or lost all lives
        void gameOver() {  resetAll() ; theMomentOfGameOver = true ;  }

        bool isGameOver () const {  return theMomentOfGameOver ;  }

        // the player just ate a reincarnation fish
        void fishEaten() {  resetAll() ; theMomentOfEatenFish = true ;  }

        bool wasFishEaten () const {  return theMomentOfEatenFish ;  }

        // the player just got the crown of a planet
        void crownTaken() {  resetAll() ; theMomentOfCrownWasTaken = true ;  }

        bool wasCrownTaken () const {  return theMomentOfCrownWasTaken ;  }

        // at least one character reached Freedom, not with all crowns
        void arriveInFreedomNotWithAllCrowns() {  resetAll() ; theMomentOfArrivalInFreedomNotWithAllCrowns = true ;  }

        bool arrivedInFreedomNotWithAllCrowns () const {  return theMomentOfArrivalInFreedomNotWithAllCrowns ;  }

        // both characters reached Freedom with all the crowns
        void finalSuccess() {  resetAll() ; theMomentWhenHeadAndHeelsAreInFreedomWithAllTheCrowns = true ;  }

        bool areHeadAndHeelsInFreedomWithAllTheCrowns () const {  return theMomentWhenHeadAndHeelsAreInFreedomWithAllTheCrowns ;  }
        bool isFinalSuccess () const {  return areHeadAndHeelsInFreedomWithAllTheCrowns() ;  }

        bool isThereAny() const
        {
                return ( theMomentOfGameOver
                         || theMomentOfEatenFish
                            || theMomentOfCrownWasTaken
                               || theMomentOfArrivalInFreedomNotWithAllCrowns
                                  || theMomentWhenHeadAndHeelsAreInFreedomWithAllTheCrowns ) ;
        }

        void resetAll()
        {
                theMomentOfGameOver = false ;
                theMomentOfEatenFish = false ;
                theMomentOfCrownWasTaken = false ;
                theMomentOfArrivalInFreedomNotWithAllCrowns = false ;
                theMomentWhenHeadAndHeelsAreInFreedomWithAllTheCrowns = false ;
        }

        std::string toString () const
        {
                std::string whichMoment = "the key moment :" ;

                if ( theMomentOfGameOver ) whichMoment += " game over" ;
                if ( theMomentOfEatenFish ) whichMoment += " ate fish" ;
                if ( theMomentOfCrownWasTaken ) whichMoment += " taken crown" ;
                if ( theMomentOfArrivalInFreedomNotWithAllCrowns ) whichMoment += " in Freedom without all crowns" ;
                if ( theMomentWhenHeadAndHeelsAreInFreedomWithAllTheCrowns ) whichMoment += " Head and Heels in Freedom with all crowns" ;
                if ( ! isThereAny() ) whichMoment += " none" ;

                return whichMoment ;
        }

} ;

#endif
