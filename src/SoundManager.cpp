
#include "SoundManager.hpp"
#include "Ism.hpp"

#include <tinyxml2.h>

using isomot::SoundManager;

#ifdef DEBUG
#  define DEBUG_SOUNDS  0
#endif


SoundManager* SoundManager::instance = nilPointer ;


SoundManager::SoundManager( )
        : effectsVolume( 80 )
        , musicVolume( 75 )
{
        allegro::initAudio ();

        activityToString[ Activity::Wait ] = "wait";
        activityToString[ Activity::Push ] = "push";
        activityToString[ Activity::Move ] = "move";
        activityToString[ Activity::MoveNorth ] = activityToString[ Activity::MoveSouth ] = "move";
        activityToString[ Activity::MoveEast ] = activityToString[ Activity::MoveWest ] = "move";
        activityToString[ Activity::MoveNortheast ] = activityToString[ Activity::MoveSoutheast ] = "move";
        activityToString[ Activity::MoveSouthwest ] = activityToString[ Activity::MoveNorthwest ] = "move";
        activityToString[ Activity::MoveUp ] = activityToString[ Activity::MoveDown ] = "move";
        activityToString[ Activity::AutoMove ] = "move";
        activityToString[ Activity::AutoMoveNorth ] = activityToString[ Activity::AutoMoveSouth ] = "move";
        activityToString[ Activity::AutoMoveEast ] = activityToString[ Activity::AutoMoveWest ] = "move";
        activityToString[ Activity::Blink ] = "blink";
        activityToString[ Activity::Jump ] = "jump";
        activityToString[ Activity::RegularJump ] = activityToString[ Activity::HighJump ] = "jump";
        activityToString[ Activity::Fall ] = "fall";
        activityToString[ Activity::Glide ] = "fall";
        activityToString[ Activity::TakeItem ] = activityToString[ Activity::TakeAndJump ] = "take";
        activityToString[ Activity::ItemTaken ] = "taken";
        activityToString[ Activity::DropItem ] = activityToString[ Activity::DropAndJump ] = "drop";
        activityToString[ Activity::DisplaceNorth ] = activityToString[ Activity::DisplaceSouth ] = "push";
        activityToString[ Activity::DisplaceEast ] = activityToString[ Activity::DisplaceWest ] = "push";
        activityToString[ Activity::DisplaceNortheast ] = activityToString[ Activity::DisplaceSoutheast ] = "push";
        activityToString[ Activity::DisplaceSouthwest ] = activityToString[ Activity::DisplaceNorthwest ] = "push";
        activityToString[ Activity::DisplaceUp ] = activityToString[ Activity::DisplaceDown ] = "push";
        activityToString[ Activity::BeginWayOutTeletransport ] = activityToString[ Activity::WayOutTeletransport ] = "teleport-out";
        activityToString[ Activity::BeginWayInTeletransport ] = activityToString[ Activity::WayInTeletransport ] = "teleport-in";
        activityToString[ Activity::ForceDisplaceNorth ] = activityToString[ Activity::ForceDisplaceSouth ] = "force";
        activityToString[ Activity::ForceDisplaceEast ] = activityToString[ Activity::ForceDisplaceWest ] = "force";
        activityToString[ Activity::CancelDisplaceNorth ] = activityToString[ Activity::CancelDisplaceSouth ] = "move";
        activityToString[ Activity::CancelDisplaceEast ] = activityToString[ Activity::CancelDisplaceWest ] = "move";
        activityToString[ Activity::MeetMortalItem ] = "death";
        activityToString[ Activity::Vanish ] = "vanish";
        activityToString[ Activity::FireDoughnut ] = "donut";
        activityToString[ Activity::Rebound ] = "rebound";
        activityToString[ Activity::SwitchIt ] = "switch";
        activityToString[ Activity::Collision ] = "collision";
        activityToString[ Activity::IsActive ] = "active";
        activityToString[ Activity::Mistake ] = "mistake";
}

SoundManager::~SoundManager( )
{
        for ( std::map< std::string, allegro::Sample* >::iterator i = samples.begin (); i != samples.end (); ++ i )
                delete i->second ;
}

SoundManager* SoundManager::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new SoundManager();
        }

        return instance;
}

void SoundManager::readSounds( const std::string& xmlFile )
{
        // read list of sounds from XML file
        tinyxml2::XMLDocument sounds;
        tinyxml2::XMLError result = sounds.LoadFile( ( isomot::sharePath() + xmlFile ).c_str () );
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

        allegro::Sample* sample = allegro::Sample::loadFromFile( isomot::pathToFile( isomot::sharePath() + sampleFile ) );

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
                std::cout << ( loop ? "looping" : "playing" ) << " sound for event \"" << translateActivityToString( activity ) << "\" of " << label
                                << " ( voice " << sample->getVoice() << " @ " << sample->getFrequency() << " Hz"
                                        << " volume " << sample->getVolume() << " )" << std::endl ;
#endif
        }

        for ( std::map< std::string, allegro::Sample* >::const_iterator j = samples.begin (); j != samples.end (); ++ j )
                        if ( j->second != nilPointer ) j->second->binVoiceIfNotPlaying() ;
}

void SoundManager::stop( const std::string& label, const ActivityOfItem& activity )
{
        allegro::Sample* sample = getSampleFor( label, activity );

        if ( sample != nilPointer && sample->isNotNil() )
        {
#if defined( DEBUG_SOUNDS ) && DEBUG_SOUNDS
                std::cout << "stopping sound for event \"" << translateActivityToString( activity ) << "\" of " << label
                                << " ( voice " << sample->getVoice() << " )" << std::endl ;
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
        std::string fullName = isomot::pathToFile( isomot::sharePath() + oggFile );

         // let’s play the same again? yep, when it’s finished playing
        if ( oggPlayingThread.getFilePlaying() != fullName || ! oggPlayingThread.isPlaying() )
        {
                // it is playback of just single Ogg yet
                oggPlayingThread.stop();

                allegro::setDigitalVolume( ( this->musicVolume * 255 ) / 99 );

                oggPlayingThread.play( fullName, loop );

                std::cout << ( loop ? "looping" : "playing" ) << " Ogg " << oggPlayingThread.getFilePlaying() << std::endl ;
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

std::string SoundManager::translateActivityToString ( const ActivityOfItem& activity )
{
        if ( activityToString.find( activity ) != activityToString.end () )
                return activityToString[ activity ];

        return "other";
}
