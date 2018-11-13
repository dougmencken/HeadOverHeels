
#include "SoundManager.hpp"
#include "Ism.hpp"

#include <tinyxml2.h>

using iso::SoundManager;

#ifdef DEBUG
#  define DEBUG_SOUNDS  0
#endif


SoundManager* SoundManager::instance = nilPointer ;


SoundManager::SoundManager( )
        : effectsVolume( 80 )
        , musicVolume( 75 )
{
        allegro::initAudio ();

        activityStrings[ Activity::Wait ] = "wait";
        activityStrings[ Activity::Push ] = "push";
        activityStrings[ Activity::Move ] = "move";
        activityStrings[ Activity::MoveNorth ] = activityStrings[ Activity::MoveSouth ] = "move";
        activityStrings[ Activity::MoveEast ] = activityStrings[ Activity::MoveWest ] = "move";
        activityStrings[ Activity::MoveNortheast ] = activityStrings[ Activity::MoveSoutheast ] = "move";
        activityStrings[ Activity::MoveSouthwest ] = activityStrings[ Activity::MoveNorthwest ] = "move";
        activityStrings[ Activity::MoveUp ] = activityStrings[ Activity::MoveDown ] = "move";
        activityStrings[ Activity::AutoMove ] = "move";
        activityStrings[ Activity::AutoMoveNorth ] = activityStrings[ Activity::AutoMoveSouth ] = "move";
        activityStrings[ Activity::AutoMoveEast ] = activityStrings[ Activity::AutoMoveWest ] = "move";
        activityStrings[ Activity::Blink ] = "blink";
        activityStrings[ Activity::Jump ] = "jump";
        activityStrings[ Activity::RegularJump ] = activityStrings[ Activity::HighJump ] = "jump";
        activityStrings[ Activity::Fall ] = "fall";
        activityStrings[ Activity::Glide ] = "fall";
        activityStrings[ Activity::TakeItem ] = activityStrings[ Activity::TakeAndJump ] = "take";
        activityStrings[ Activity::ItemTaken ] = "taken";
        activityStrings[ Activity::DropItem ] = activityStrings[ Activity::DropAndJump ] = "drop";
        activityStrings[ Activity::DisplaceNorth ] = activityStrings[ Activity::DisplaceSouth ] = "push";
        activityStrings[ Activity::DisplaceEast ] = activityStrings[ Activity::DisplaceWest ] = "push";
        activityStrings[ Activity::DisplaceNortheast ] = activityStrings[ Activity::DisplaceSoutheast ] = "push";
        activityStrings[ Activity::DisplaceSouthwest ] = activityStrings[ Activity::DisplaceNorthwest ] = "push";
        activityStrings[ Activity::DisplaceUp ] = activityStrings[ Activity::DisplaceDown ] = "push";
        activityStrings[ Activity::BeginWayOutTeletransport ] = activityStrings[ Activity::WayOutTeletransport ] = "teleport-out";
        activityStrings[ Activity::BeginWayInTeletransport ] = activityStrings[ Activity::WayInTeletransport ] = "teleport-in";
        activityStrings[ Activity::ForceDisplaceNorth ] = activityStrings[ Activity::ForceDisplaceSouth ] = "force";
        activityStrings[ Activity::ForceDisplaceEast ] = activityStrings[ Activity::ForceDisplaceWest ] = "force";
        activityStrings[ Activity::CancelDisplaceNorth ] = activityStrings[ Activity::CancelDisplaceSouth ] = "move";
        activityStrings[ Activity::CancelDisplaceEast ] = activityStrings[ Activity::CancelDisplaceWest ] = "move";
        activityStrings[ Activity::MeetMortalItem ] = "death";
        activityStrings[ Activity::Vanish ] = "vanish";
        activityStrings[ Activity::FireDoughnut ] = "donut";
        activityStrings[ Activity::Rebound ] = "rebound";
        activityStrings[ Activity::SwitchIt ] = "switch";
        activityStrings[ Activity::Collision ] = "collision";
        activityStrings[ Activity::IsActive ] = "active";
        activityStrings[ Activity::Mistake ] = "mistake";
}

SoundManager::~SoundManager( )
{
        for ( std::map< std::string, allegro::Sample* >::iterator i = samples.begin (); i != samples.end (); ++ i )
                delete i->second ;
}

SoundManager& SoundManager::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new SoundManager();
        }

        return *instance;
}

void SoundManager::readSounds( const std::string& xmlFile )
{
        // read list of sounds from XML file
        tinyxml2::XMLDocument sounds;
        tinyxml2::XMLError result = sounds.LoadFile( ( iso::sharePath() + xmlFile ).c_str () );
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
                std::string label = item->Attribute( "label" );

                for ( tinyxml2::XMLElement* event = item->FirstChildElement( "event" ) ;
                                event != nilPointer ;
                                event = event->NextSiblingElement( "event" ) )
                {
                        std::string activity = event->Attribute( "activity" );
                        std::string file = event->FirstChildElement( "file" )->FirstChild()->ToText()->Value();

                        addSound( label, activity, file );
                }
        }

        std::cout << "read list of sounds from " << xmlFile << std::endl ;
}

void SoundManager::addSound( const std::string& label, const std::string& activity, const std::string& sampleFile )
{
        this->sounds[ label ][ activity ] = sampleFile ;

#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
        std::cout << "sound \"" << sampleFile << "\" for activity \"" << activity << "\" of \"" << label << "\"" << std::endl ;
#endif

        if ( this->samples.find( sampleFile ) != this->samples.end () )
        {
                return ;
        }

        allegro::Sample* sample = allegro::Sample::loadFromFile( iso::pathToFile( iso::sharePath(), sampleFile ) );

        if ( sample != nilPointer && sample->isNotNil() )
        {
#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
                std::cout << "+ sample is read from file \"" << sampleFile << "\"" << std::endl ;
#endif
                this->samples[ sampleFile ] = sample;
        }
        else
        {
                std::cerr << "can’t read sound \"" << sampleFile << "\"" << std::endl ;
        }
}

void SoundManager::play( const std::string& label, const ActivityOfItem& activity, bool loop )
{
        allegro::Sample* sample = getSampleFor( label, activity );

        if ( sample != nilPointer && sample->isNotNil() && sample->getVoice() < 0 )
        {
                sample->setVolume( ( this->effectsVolume * 255 ) / 99 );

                if ( ! loop )
                        sample->play();
                else
                        sample->loop();

#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
                std::cout << ( loop ? "looping" : "playing" ) << " sound for event \"" << activityToString( activity )
                                << "\" of \"" << label << "\"" << std::endl ;
#endif
        }

        for ( std::map< std::string, allegro::Sample* >::const_iterator j = samples.begin (); j != samples.end (); ++ j )
                        if ( j->second != nilPointer ) j->second->neatenIfNotPlaying() ;
}

void SoundManager::stop( const std::string& label, const ActivityOfItem& activity )
{
        allegro::Sample* sample = getSampleFor( label, activity );

        if ( sample != nilPointer && sample->isNotNil() )
        {
#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
                std::cout << "stopping sound for event \"" << activityToString( activity ) << "\" of \"" << label << "\"" << std::endl ;
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

void SoundManager::playOgg ( const std::string& oggFile, bool loop )
{
        std::string fullName = iso::pathToFile( iso::sharePath(), oggFile );

         // let’s play the same again? yep, when it’s finished playing
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

allegro::Sample* SoundManager::getSampleFor( const std::string& label, const std::string& event )
{
        for ( std::map< std::string, std::map< std::string, std::string > >::const_iterator i = sounds.begin (); i != sounds.end (); ++ i )
                if ( label == i->first )
                        for ( std::map< std::string, std::string >::const_iterator j = i->second.begin (); j != i->second.end (); ++ j )
                                if ( event == j->first )
                                        if ( samples.find( j->second ) != samples.end () )
                                                return samples[ j->second ] ;

        return allegro::Sample::nilSample() ;
}

std::string SoundManager::activityToString ( const ActivityOfItem& activity )
{
        if ( activityStrings.find( activity ) != activityStrings.end () )
                return activityStrings[ activity ];

        return "other";
}
