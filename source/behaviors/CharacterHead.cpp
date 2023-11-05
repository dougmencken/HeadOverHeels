
#include "CharacterHead.hpp"

#include "Item.hpp"
#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

CharacterHead::CharacterHead( const ItemPtr & item, const std::string & behavior ) :
        PlayerControlled( item, behavior )
{
        jumpPhases = 20;
        highJumpPhases = 19;

        // salto normal
        for ( unsigned int i = 0; i < jumpPhases; i++ )
        {
                jumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 3 ) );
        }

        // salto largo
        for ( unsigned int i = 0; i < highJumpPhases; i++ )
        {
                highJumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 4 ) );
        }

        // fotogramas de caída
        fallFrames[ "north" ] = 8;
        fallFrames[ "south" ] = 16;
        fallFrames[ "east" ] = 12;
        fallFrames[ "west" ] = 17;

        // fotogramas de parpadeo
        blinkFrames[ "north" ] = 8;
        blinkFrames[ "south" ] = 19;
        blinkFrames[ "east" ] = 12;
        blinkFrames[ "west" ] = 20;

        // activate chronometers
        speedTimer->go ();
        fallTimer->go ();
        glideTimer->go ();
        timerForBlinking->go ();
}

CharacterHead::~CharacterHead( )
{
}

bool CharacterHead::update ()
{
        AvatarItem & characterItem = dynamic_cast< AvatarItem & >( * this->item );

        if ( characterItem.hasShield() )
        {
                characterItem.decrementShieldOverTime () ;
        }

        // change height for climbing bars easily
        characterItem.setHeight( activity == activities::Activity::Fall || activity == activities::Activity::Glide ? 23 : 24 );

        switch ( activity )
        {
                case activities::Activity::Wait:
                        wait( characterItem );
                        break;

                case activities::Activity::AutoMoveNorth:
                case activities::Activity::AutoMoveSouth:
                case activities::Activity::AutoMoveEast:
                case activities::Activity::AutoMoveWest:
                        autoMove( characterItem );
                        break;

                case activities::Activity::MoveNorth:
                case activities::Activity::MoveSouth:
                case activities::Activity::MoveEast:
                case activities::Activity::MoveWest:
                        move( characterItem );
                        break;

                case activities::Activity::DisplaceNorth:
                case activities::Activity::DisplaceSouth:
                case activities::Activity::DisplaceEast:
                case activities::Activity::DisplaceWest:
                case activities::Activity::DisplaceNortheast:
                case activities::Activity::DisplaceSoutheast:
                case activities::Activity::DisplaceSouthwest:
                case activities::Activity::DisplaceNorthwest:
                case activities::Activity::ForceDisplaceNorth:
                case activities::Activity::ForceDisplaceSouth:
                case activities::Activity::ForceDisplaceEast:
                case activities::Activity::ForceDisplaceWest:
                        displace( characterItem );
                        break;

                case activities::Activity::CancelDisplaceNorth:
                case activities::Activity::CancelDisplaceSouth:
                case activities::Activity::CancelDisplaceEast:
                case activities::Activity::CancelDisplaceWest:
                        cancelDisplace( characterItem );
                        break;

                case activities::Activity::Fall:
                        fall( characterItem );
                        break;

                case activities::Activity::Jump:
                case activities::Activity::RegularJump:
                case activities::Activity::HighJump:
                        jump( characterItem );
                        break;

                case activities::Activity::BeginTeletransportation:
                        enterTeletransport( characterItem );
                        break;
                case activities::Activity::EndTeletransportation:
                        exitTeletransport( characterItem );
                        break;

                case activities::Activity::MeetMortalItem:
                case activities::Activity::Vanish:
                        collideWithMortalItem( characterItem );
                        break;

                case activities::Activity::Glide:
                        glide( characterItem );
                        break;

                case activities::Activity::Blink:
                        blink( characterItem );
                        break;

                default:
                        ;
        }

        // play sound for current activity
        SoundManager::getInstance().play( characterItem.getOriginalKind(), activity );

        return false;
}

void CharacterHead::behave ()
{
        AvatarItem & characterItem = dynamic_cast< AvatarItem & >( * this->item );
        InputManager& input = InputManager::getInstance();

        // if it’s not a move by inertia or some other exotic activity
        if ( activity != activities::Activity::AutoMoveNorth && activity != activities::Activity::AutoMoveSouth
                        && activity != activities::Activity::AutoMoveEast && activity != activities::Activity::AutoMoveWest
                        && activity != activities::Activity::BeginTeletransportation && activity != activities::Activity::EndTeletransportation
                        && activity != activities::Activity::MeetMortalItem && activity != activities::Activity::Vanish )
        {
                // when waiting or blinking
                if ( activity == activities::Activity::Wait || activity == activities::Activity::Blink )
                {
                        if ( input.jumpTyped() )
                        {
                                // jump or teleport
                                characterItem.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        characterItem.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer ?
                                                activities::Activity::BeginTeletransportation : activities::Activity::Jump ;
                        }
                        else if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = activities::Activity::MoveNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = activities::Activity::MoveSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = activities::Activity::MoveEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = activities::Activity::MoveWest;
                        }
                }
                // already moving
                else if ( activity == activities::Activity::MoveNorth || activity == activities::Activity::MoveSouth ||
                        activity == activities::Activity::MoveEast || activity == activities::Activity::MoveWest )
                {
                        if ( input.jumpTyped() )
                        {
                                // look for teletransport below
                                characterItem.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        characterItem.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer ?
                                                activities::Activity::BeginTeletransportation : activities::Activity::Jump ;
                        }
                        else if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = activities::Activity::MoveNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = activities::Activity::MoveSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = activities::Activity::MoveEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = activities::Activity::MoveWest;
                        }
                        else if ( ! input.anyMoveTyped() )
                        {
                                SoundManager::getInstance().stop( characterItem.getKind(), activity );
                                activity = activities::Activity::Wait;
                        }
                }
                // if you are being displaced
                else if ( activity == activities::Activity::DisplaceNorth || activity == activities::Activity::DisplaceSouth ||
                        activity == activities::Activity::DisplaceEast || activity == activities::Activity::DisplaceWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = activities::Activity::Jump;
                        }
                        else if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = activities::Activity::MoveNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = activities::Activity::MoveSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = activities::Activity::MoveEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = activities::Activity::MoveWest;
                        }
                }
                // if you are being forcibly displaced
                else if ( activity == activities::Activity::ForceDisplaceNorth || activity == activities::Activity::ForceDisplaceSouth ||
                        activity == activities::Activity::ForceDisplaceEast || activity == activities::Activity::ForceDisplaceWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = activities::Activity::Jump;
                        }
                        // user moves while displacing
                        // cancel displace when moving in direction opposite to displacement
                        else if ( input.movenorthTyped() )
                        {
                                activity = ( activity == activities::Activity::ForceDisplaceSouth ? activities::Activity::CancelDisplaceSouth : activities::Activity::MoveNorth );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = ( activity == activities::Activity::ForceDisplaceNorth ? activities::Activity::CancelDisplaceNorth : activities::Activity::MoveSouth );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = ( activity == activities::Activity::ForceDisplaceWest ? activities::Activity::CancelDisplaceWest : activities::Activity::MoveEast );
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = ( activity == activities::Activity::ForceDisplaceEast ? activities::Activity::CancelDisplaceEast : activities::Activity::MoveWest );
                        }
                }
                else if ( activity == activities::Activity::Jump || activity == activities::Activity::RegularJump || activity == activities::Activity::HighJump )
                {
                        if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // Head may change orientation when jumping
                        else if ( input.movenorthTyped() )
                        {
                                characterItem.changeOrientation( "north" );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                characterItem.changeOrientation( "south" );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                characterItem.changeOrientation( "east" );
                        }
                        else if ( input.movewestTyped() )
                        {
                                characterItem.changeOrientation( "west" );
                        }
                }
                else if ( activity == activities::Activity::Fall )
                {
                        if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // entonces Head planea
                        else if ( input.anyMoveTyped() )
                        {
                                activity = activities::Activity::Glide;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between two grid items
                if ( activity == activities::Activity::Glide )
                {
                        if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // Head may change orientation when gliding
                        else if ( input.movenorthTyped() )
                        {
                                characterItem.changeOrientation( "north" );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                characterItem.changeOrientation( "south" );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                characterItem.changeOrientation( "east" );
                        }
                        else if ( input.movewestTyped() )
                        {
                                characterItem.changeOrientation( "west" );
                        }
                        else if ( ! input.anyMoveTyped() )
                        {
                                activity = activities::Activity::Fall;
                        }
                }
        }
}

void CharacterHead::wait( AvatarItem & characterItem )
{
        characterItem.wait();

        if ( timerForBlinking->getValue() >= ( rand() % 4 ) + 5 )
        {
                timerForBlinking->reset();
                activity = activities::Activity::Blink;
        }

        if ( activities::Falling::getInstance().fall( this ) )
        {
                speedTimer->reset();
                activity = activities::Activity::Fall;
        }
}

void CharacterHead::blink( AvatarItem & characterItem )
{
        double timeToBlink = timerForBlinking->getValue();

        if ( ( timeToBlink > 0.0 && timeToBlink < 0.050 ) || ( timeToBlink > 0.400 && timeToBlink < 0.450 ) )
        {
                characterItem.changeFrame( blinkFrames[ characterItem.getOrientation() ] );
        }
        else if ( ( timeToBlink > 0.250 && timeToBlink < 0.300 ) || ( timeToBlink > 0.750 && timeToBlink < 0.800 ) )
        {
        }
        else if ( timeToBlink > 0.800 )
        {
                timerForBlinking->reset();
                activity = activities::Activity::Wait;
        }
}

}
