
#include "SoundManager.hpp"

#include "util.hpp"
#include "ospaths.hpp"

#include <tinyxml2.h>

#ifdef DEBUG
#  define DEBUG_SOUNDS  0
#endif


/* static */ SoundManager * SoundManager::instance = nilPointer ;

/* static */ std::string SoundManager::audioInterface = "auto" ;


SoundManager::SoundManager( const std::string & audioInterface )
        : audioInitialized( false )
        , effectsVolume( 80 )
        , musicVolume( 75 )
{
        audioInitialized = allegro::initAudio( audioInterface ) ;

        activityStrings[ activities::Activity::Wait ] = "wait";
        activityStrings[ activities::Activity::Push ] = "push";
        activityStrings[ activities::Activity::Move ] = "move";
        activityStrings[ activities::Activity::MoveNorth ] = activityStrings[ activities::Activity::MoveSouth ] = "move";
        activityStrings[ activities::Activity::MoveEast ] = activityStrings[ activities::Activity::MoveWest ] = "move";
        activityStrings[ activities::Activity::MoveNortheast ] = activityStrings[ activities::Activity::MoveSoutheast ] = "move";
        activityStrings[ activities::Activity::MoveSouthwest ] = activityStrings[ activities::Activity::MoveNorthwest ] = "move";
        activityStrings[ activities::Activity::MoveUp ] = activityStrings[ activities::Activity::MoveDown ] = "move";
        activityStrings[ activities::Activity::AutoMove ] = "move";
        activityStrings[ activities::Activity::AutoMoveNorth ] = activityStrings[ activities::Activity::AutoMoveSouth ] = "move";
        activityStrings[ activities::Activity::AutoMoveEast ] = activityStrings[ activities::Activity::AutoMoveWest ] = "move";
        activityStrings[ activities::Activity::Blink ] = "blink";
        activityStrings[ activities::Activity::Jump ] = "jump";
        activityStrings[ activities::Activity::RegularJump ] = activityStrings[ activities::Activity::HighJump ] = "jump";
        activityStrings[ activities::Activity::Fall ] = "fall";
        activityStrings[ activities::Activity::Glide ] = "fall";
        activityStrings[ activities::Activity::TakeItem ] = activityStrings[ activities::Activity::TakeAndJump ] = "take";
        activityStrings[ activities::Activity::ItemTaken ] = "taken";
        activityStrings[ activities::Activity::DropItem ] = activityStrings[ activities::Activity::DropAndJump ] = "drop";
        activityStrings[ activities::Activity::DisplaceNorth ] = activityStrings[ activities::Activity::DisplaceSouth ] = "push";
        activityStrings[ activities::Activity::DisplaceEast ] = activityStrings[ activities::Activity::DisplaceWest ] = "push";
        activityStrings[ activities::Activity::DisplaceNortheast ] = activityStrings[ activities::Activity::DisplaceSoutheast ] = "push";
        activityStrings[ activities::Activity::DisplaceSouthwest ] = activityStrings[ activities::Activity::DisplaceNorthwest ] = "push";
        activityStrings[ activities::Activity::DisplaceUp ] = activityStrings[ activities::Activity::DisplaceDown ] = "push";
        activityStrings[ activities::Activity::BeginWayOutTeletransport ] = activityStrings[ activities::Activity::WayOutTeletransport ] = "teleport-out";
        activityStrings[ activities::Activity::BeginWayInTeletransport ] = activityStrings[ activities::Activity::WayInTeletransport ] = "teleport-in";
        activityStrings[ activities::Activity::ForceDisplaceNorth ] = activityStrings[ activities::Activity::ForceDisplaceSouth ] = "force";
        activityStrings[ activities::Activity::ForceDisplaceEast ] = activityStrings[ activities::Activity::ForceDisplaceWest ] = "force";
        activityStrings[ activities::Activity::CancelDisplaceNorth ] = activityStrings[ activities::Activity::CancelDisplaceSouth ] = "move";
        activityStrings[ activities::Activity::CancelDisplaceEast ] = activityStrings[ activities::Activity::CancelDisplaceWest ] = "move";
        activityStrings[ activities::Activity::MeetMortalItem ] = "death";
        activityStrings[ activities::Activity::Vanish ] = "vanish";
        activityStrings[ activities::Activity::FireDoughnut ] = "donut";
        activityStrings[ activities::Activity::Rebound ] = "rebound";
        activityStrings[ activities::Activity::SwitchIt ] = "switch";
        activityStrings[ activities::Activity::Collision ] = "collision";
        activityStrings[ activities::Activity::IsActive ] = "active";
        activityStrings[ activities::Activity::Mistake ] = "mistake";
}

SoundManager::~SoundManager( )
{
        for ( std::map< std::string, allegro::Sample* >::iterator i = samples.begin (); i != samples.end (); ++ i )
                delete i->second ;
}

SoundManager& SoundManager::getInstance()
{
        if ( instance == nilPointer )
                instance = new SoundManager( SoundManager::audioInterface ) ;

        return *instance ;
}

void SoundManager::readSounds( const std::string& xmlFile )
{
        // read list of sounds from XML file
        tinyxml2::XMLDocument sounds;
        tinyxml2::XMLError result = sounds.LoadFile( ( ospaths::sharePath() + xmlFile ).c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t read list of sounds from \"" << xmlFile << "\"" << std::endl ;
                return;
        }

        tinyxml2::XMLElement* root = sounds.FirstChildElement( "sounds" );

        for ( tinyxml2::XMLElement* item = root->FirstChildElement( "item" ) ;
                        item != nilPointer ;
                        item = item->NextSiblingElement( "item" ) )
        {
                std::string kindOfItem = item->Attribute( "kind" );

                for ( tinyxml2::XMLElement* event = item->FirstChildElement( "event" ) ;
                                event != nilPointer ;
                                event = event->NextSiblingElement( "event" ) )
                {
                        std::string activity = event->Attribute( "activity" );
                        std::string file = event->FirstChildElement( "file" )->FirstChild()->ToText()->Value();

                        addSound( kindOfItem, activity, file );
                }
        }

        std::cout << "read list of sounds from " << xmlFile << std::endl ;
}

void SoundManager::addSound( const std::string & item, const std::string& activity, const std::string& sampleFile )
{
        this->sounds[ item ][ activity ] = sampleFile ;

#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
        std::cout << "sound \"" << sampleFile << "\" for activity \"" << activity << "\" of \"" << item << "\"" << std::endl ;
#endif

        if ( this->samples.find( sampleFile ) != this->samples.end () ) // it's already here
                return ;

        std::string pathToSample = ospaths::pathToFile( ospaths::sharePath(), sampleFile ) ;
        allegro::Sample* sample = allegro::Sample::loadFromFile( pathToSample );

        if ( sample != nilPointer && sample->isNotNil() )
        {
#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
                std::cout << "+ sample is read from file \"" << sampleFile << "\"" << std::endl ;
#endif
                this->samples[ sampleFile ] = sample;
        }
        else
        {
                std::cerr << "can’t read sound \"" << sampleFile << "\" (" << pathToSample << ")" << std::endl ;
        }
}

void SoundManager::play( const std::string & item, const ActivityOfItem & activity, bool loop )
{
        allegro::Sample* sample = getSampleFor( item, activity );

        if ( sample != nilPointer && sample->isNotNil() && sample->getVoice() < 0 )
        {
                sample->setVolume( ( this->effectsVolume * 255 ) / 99 );

                if ( ! loop )
                        sample->play();
                else
                        sample->loop();

#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
                std::cout << ( loop ? "looping" : "playing" ) << " sound for event \"" << activityToString( activity )
                                << "\" of \"" << item << "\"" << std::endl ;
#endif
        }

        for ( std::map< std::string, allegro::Sample* >::const_iterator j = samples.begin (); j != samples.end (); ++ j )
                        if ( j->second != nilPointer ) j->second->neatenIfNotPlaying() ;
}

void SoundManager::stop( const std::string & item, const ActivityOfItem & activity )
{
        allegro::Sample* sample = getSampleFor( item, activity );

        if ( sample != nilPointer && sample->isNotNil() )
        {
#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
                std::cout << "stopping sound for event \"" << activityToString( activity ) << "\" of \"" << item << "\"" << std::endl ;
#endif
                sample->stop();
        }
}

void SoundManager::stopEverySound ()
{
        for ( std::map< std::string, allegro::Sample* >::const_iterator j = samples.begin (); j != samples.end (); ++ j )
        {
                if ( j->second != nilPointer ) j->second->stop();
        }
}

void SoundManager::playOgg ( const std::string & oggFile, bool loop )
{
        std::string fullName = ospaths::pathToFile( ospaths::sharePath(), oggFile );

         // to play the same again? yep, when it’s finished playing
        if ( oggPlayer.getFilePlaying() != fullName || ! oggPlayer.isPlaying() )
        {
                // it is playback of just single Ogg yet
                oggPlayer.stop();

                allegro::setDigitalVolume( ( this->musicVolume * 255 ) / 99 );

                oggPlayer.play( fullName, loop );

                if ( ! oggPlayer.getFilePlaying().empty() )
                        std::cout << ( loop ? "looping" : "playing" ) << " Ogg " << oggPlayer.getFilePlaying() << std::endl ;
        }
}

void SoundManager::setVolumeOfMusic( unsigned int volume )
{
          this->musicVolume = ( volume <= 99 ) ? volume : 99 ;
          allegro::setDigitalVolume( ( this->musicVolume * 255 ) / 99 );
}

allegro::Sample* SoundManager::getSampleFor( const std::string & item, const std::string & event )
{
        for ( std::map< std::string, std::map< std::string, std::string > >::const_iterator i = sounds.begin (); i != sounds.end (); ++ i )
                if ( item == i->first )
                        for ( std::map< std::string, std::string >::const_iterator j = i->second.begin (); j != i->second.end (); ++ j )
                                if ( event == j->first )
                                        if ( samples.find( j->second ) != samples.end () )
                                                return samples[ j->second ] ;

        return allegro::Sample::nilSample() ;
}

std::string SoundManager::activityToString ( const ActivityOfItem & activity )
{
        if ( activityStrings.find( activity ) != activityStrings.end () )
                return activityStrings[ activity ];

        return "other";
}
