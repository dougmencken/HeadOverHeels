
#include "SoundManager.hpp"
#include "Ism.hpp"

using isomot::SoundManager;
using isomot::SampleData;
using isomot::SoundData;
using isomot::EqualSoundData;

SoundManager* SoundManager::instance = nilPointer ;


SoundManager::SoundManager( )
        : fileName( std::string() ),
          oggPlayer( nilPointer ),
          oggStream( nilPointer ),
          effectsVolume( 80 ),
          musicVolume( 75 )
{
        alogg_init();
        install_timer();

        int soundSystems[] = {  DIGI_AUTODETECT,
        # ifdef __gnu_linux__
                                DIGI_ALSA,
                                DIGI_OSS,
                                DIGI_ESD,
                                DIGI_ARTS,
                                DIGI_JACK,
        # endif
                                DIGI_NONE  };
        int result = -1;
        int index = 0;

        while ( result == -1 )
        {
                result = install_sound( soundSystems[ index++ ], MIDI_NONE, NULL );
        }
}

SoundManager::~SoundManager( )
{
        alogg_exit();
}

SoundManager* SoundManager::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new SoundManager();
        }

        return instance;
}

void SoundManager::readListOfSounds( const std::string& fileName )
{
        // read list of sounds from XML file
        try
        {
                std::auto_ptr< sxml::SoundsXML > soundsXML( sxml::sounds( ( isomot::sharePath() + fileName ).c_str () ) );

                for ( sxml::SoundsXML::item_const_iterator i = soundsXML->item().begin (); i != soundsXML->item().end (); ++i )
                {
                        SoundData soundData( ( *i ).label() );

                        for ( sxml::item::event_const_iterator s = ( *i ).event().begin (); s != ( *i ).event().end (); ++s )
                        {
                                soundData.addSound( ( *s ).id(), ( *s ).file() );
                        }

                        this->soundData.push_back( soundData );
                }

                std::cout << "read list of sounds from " << fileName << std::endl ;
        }
        catch ( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
        catch ( const std::exception& e )
        {
                std::cerr << e.what () << std::endl ;
        }
}

void SoundManager::play( const std::string& label, const ActivityOfItem& activity, bool loop )
{
        SampleData* sampleData = this->findSample( label, activity );

        if ( sampleData != nilPointer && sampleData->voice == -1 )
        {
                sampleData->voice = allocate_voice( sampleData->sample );
                voice_set_volume( sampleData->voice, ( this->effectsVolume * 255 ) / 100 );
                voice_set_playmode( sampleData->voice, PLAYMODE_PLAY /* PLAYMODE_BACKWARD */ );
                if ( loop )
                {
                        voice_set_playmode( sampleData->voice, PLAYMODE_LOOP );
                }
                voice_start( sampleData->voice );
        }

        for ( std::list< SoundData >::iterator i = soundData.begin (); i != soundData.end (); ++i )
        {
                for ( __gnu_cxx::hash_map< std::string, SampleData >::iterator j = ( *i ).table.begin (); j != ( *i ).table.end (); ++j )
                {
                        if ( voice_get_position( j->second.voice ) == -1 )
                        {
                                deallocate_voice( j->second.voice );
                                j->second.voice = -1;
                        }
                }
        }
}

void SoundManager::stop( const std::string& label, const ActivityOfItem& activity )
{
        SampleData* sampleData = this->findSample( label, activity );

        if ( sampleData != nilPointer )
        {
                stop_sample( sampleData->sample );
        }
}

void SoundManager::stopEverySound ()
{
        for ( std::list< SoundData >::iterator i = soundData.begin (); i != soundData.end (); ++i )
        {
                for ( __gnu_cxx::hash_map< std::string, SampleData >::iterator j = ( *i ).table.begin (); j != ( *i ).table.end (); ++j )
                {
                        stop_sample( j->second.sample );
                        if ( voice_get_position( j->second.voice ) == -1 )
                        {
                                deallocate_voice( j->second.voice );
                                j->second.voice = -1;
                        }
                }
        }
}

void SoundManager::playOgg ( const std::string& fileName, bool loop )
{
        if ( ! this->isPlayingOgg ( fileName ) ) // let’s play the same again? nope
        {
                // create buffer of 40 KiB
                const size_t lengthOfbuffer = 40 * 1024;

                set_volume( ( this->musicVolume * 255 ) / 100, 0 );

                this->oggStream = alogg_start_streaming( isomot::pathToFile( isomot::sharePath() + fileName ), lengthOfbuffer );
                if ( this->oggStream != nilPointer )
                {
                        alogg_thread* oggThread = nilPointer;
                        if ( loop )
                                oggThread = alogg_create_thread_which_loops( this->oggStream, isomot::pathToFile( isomot::sharePath() + fileName ), lengthOfbuffer );
                        else
                                oggThread = alogg_create_thread( this->oggStream );

                        if ( oggThread != nilPointer )
                        {
                                if ( this->oggPlayer != nilPointer )
                                        this->stopAnyOgg (); // it is playback of just single Ogg yet
                                                             // or previous oggPlayer will be lost
                                                             // and if it is the one which loops forever it would never ever end

                                this->oggPlaying = fileName;
                                this->oggPlayer = oggThread;

                                if ( loop )
                                        std::cout << "looping";
                                else
                                        std::cout << "playing";
                                std::cout << " Ogg " << fileName << std::endl ;
                        }
                }
                else
                {
                        std::cerr << "can’t play Ogg " << fileName << std::endl ;
                }
        }
}

void SoundManager::stopAnyOgg ()
{
        if ( oggPlayer != nilPointer )
        {
                if ( oggPlayer->stop == 0 )
                {
                        alogg_stop_thread( oggPlayer );
                        while ( alogg_is_thread_alive( oggPlayer ) ) ;
                        alogg_join_thread( oggPlayer );
                        alogg_destroy_thread( oggPlayer );
                        oggPlayer->stop = 1;

                        std::cout << "Ogg playback is off" << std::endl ;
                }

                oggPlayer = nilPointer;
                oggStream = nilPointer;
                oggPlaying.clear();
        }
}

void SoundManager::setVolumeOfMusic( unsigned int volume )
{
          this->musicVolume = ( volume == 0 ? 1 : volume );
          set_volume( ( this->musicVolume * 255 ) / 100, 0 );
}

SampleData* SoundManager::findSample( const std::string& label, const ActivityOfItem& activity )
{
        std::list< SoundData >::iterator i = std::find_if( soundData.begin(), soundData.end(), std::bind2nd( EqualSoundData(), label ) );

        return i != soundData.end () ? ( *i ).find( this->translateActivityOfItemToString( activity ) ) : nilPointer;
}

std::string SoundManager::translateActivityOfItemToString ( const ActivityOfItem& activity )
{
        std::string stringOfActivity;

        switch ( activity )
        {
                case Wait:
                        stringOfActivity = "wait";
                        break;

                case Push:
                        break;

                case Move:
                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                case MoveNortheast:
                case MoveSoutheast:
                case MoveSouthwest:
                case MoveNorthwest:
                case MoveUp:
                case MoveDown:
                case AutoMove:
                case AutoMoveNorth:
                case AutoMoveSouth:
                case AutoMoveEast:
                case AutoMoveWest:
                        stringOfActivity = "move";
                        break;

                case Blink:
                        break;

                case Jump:
                case RegularJump:
                case HighJump:
                        stringOfActivity = "jump";
                        break;

                case Fall:
                case Glide:
                        stringOfActivity = "fall";
                        break;

                case TakeItem:
                case TakeAndJump:
                        stringOfActivity = "take";
                        break;

                case TakenItem:
                break;

                case DropItem:
                case DropAndJump:
                        stringOfActivity = "drop";
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                case DisplaceUp:
                case DisplaceDown:
                        stringOfActivity = "push";
                        break;

                case BeginWayOutTeletransport:
                case WayOutTeletransport:
                        stringOfActivity = "teleport-out";
                        break;

                case BeginWayInTeletransport:
                case WayInTeletransport:
                        stringOfActivity = "teleport-in";
                        break;

                case ForceDisplaceNorth:
                case ForceDisplaceSouth:
                case ForceDisplaceEast:
                case ForceDisplaceWest:
                        stringOfActivity = "force";
                        break;

                case CancelDisplaceNorth:
                case CancelDisplaceSouth:
                case CancelDisplaceEast:
                case CancelDisplaceWest:
                        stringOfActivity = "move";
                        break;

                case MeetMortalItem:
                        break;

                case Vanish:
                        stringOfActivity = "vanish";
                        break;

                case FireDoughnut:
                        stringOfActivity = "shot";
                        break;

                case Rebound:
                        stringOfActivity = "rebound";
                        break;

                case SwitchIt:
                        stringOfActivity = "switch";
                        break;

                case Collision:
                        stringOfActivity = "collision";
                        break;

                case IsActive:
                        stringOfActivity = "active";
                        break;

                case Mistake:
                        stringOfActivity = "mistake";
                        break;

                default:
                        ;
        }

        return stringOfActivity;
}

SoundData::SoundData( const std::string& label )
        : label( label )
{

}

void SoundData::addSound( const std::string& activity, const std::string& sampleFileName )
{
        SAMPLE* sample = load_sample( isomot::pathToFile( isomot::sharePath() + sampleFileName ) );

        if ( sample != nilPointer ) {
        #if defined( DEBUG ) && DEBUG
                std::cout << "got sound \"" << sampleFileName << "\" for activity \"" << activity << "\" of \"" << label << "\"" << std::endl ;
        #endif
                SampleData sampleData;
                sampleData.sample = sample;
                sampleData.voice = -1;
                this->table[ activity ] = sampleData;
        } else
                std::cerr << "can’t read sound \"" << sampleFileName << "\" for activity \"" << activity << "\" of \"" << label << "\"" << std::endl ;
}

SampleData* SoundData::find( const std::string& event )
{
        __gnu_cxx::hash_map< std::string, SampleData >::iterator i = this->table.find( event );

        return i != this->table.end () ? ( &( i->second ) ) : nilPointer;
}

bool EqualSoundData::operator()( const SoundData& soundData, const std::string& label ) const
{
        return ( soundData.getLabel() == label );
}
