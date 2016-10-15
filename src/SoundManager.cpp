#include "SoundManager.hpp"
#include "Exception.hpp"

using isomot::SoundManager;
using isomot::SampleData;
using isomot::SoundData;
using isomot::Exception;
using isomot::EqualSoundData;

SoundManager* SoundManager::instance = 0;

SoundManager::SoundManager( )
        : fileName( std::string() ),
          oggPlayer( 0 ),
          oggStream( 0 ),
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
        if ( instance == 0 )
        {
                instance = new SoundManager();
        }

        return instance;
}

void SoundManager::load( const std::string& fileName )
{
        // Carga el archivo XML especificado y almacena los datos XML en la lista
        try
        {
                std::auto_ptr< sxml::SoundsXML > soundsXML( sxml::sounds( ( isomot::sharePath() + fileName ).c_str () ) );

                for ( sxml::SoundsXML::item_const_iterator i = soundsXML->item().begin (); i != soundsXML->item().end (); ++i )
                {
                        SoundData soundData( ( *i ).label() );

                        for ( sxml::item::state_const_iterator s = ( *i ).state().begin (); s != ( *i ).state().end (); ++s )
                        {
                                soundData.add( ( *s ).id(), ( *s ).file() );
                        }

                        this->soundData.push_back( soundData );
                }
        }
        catch ( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
        catch ( const Exception& e )
        {
                std::cerr << e.what () << std::endl ;
        }
}

void SoundManager::play( short label, const StateId& stateId, bool loop )
{
        SampleData* sampleData = this->findSample( label, stateId );

        if ( sampleData != 0 && sampleData->voice == -1 )
        {
                deallocate_voice( sampleData->voice );
                sampleData->voice = allocate_voice( sampleData->sample );
                voice_set_volume( sampleData->voice, ( this->effectsVolume * 255 ) / 100 );
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
                                ( &( j->second) )->voice = -1;
                        }
                }
        }
}

void SoundManager::stop( short label, const StateId& stateId )
{
        SampleData* sampleData = this->findSample( label, stateId );

        if( sampleData != 0 )
        {
                stop_sample( sampleData->sample );
        }
}

void SoundManager::stopAllSounds()
{
        for ( std::list< SoundData >::iterator i = soundData.begin (); i != soundData.end (); ++i )
        {
                for ( __gnu_cxx::hash_map< std::string, SampleData >::iterator j = ( *i ).table.begin (); j != ( *i ).table.end (); ++j )
                {
                        stop_sample( j->second.sample );
                        if ( voice_get_position( j->second.voice ) == -1 )
                        {
                                deallocate_voice( j->second.voice );
                                ( &( j->second ) )->voice = -1;
                        }
                }
        }
}

void SoundManager::playOgg ( const std::string& fileName, bool loop )
{
        if ( ! this->isPlayingOgg ( fileName ) ) // let’s play the same again? nope
        {
                // Se crea un buffer de 40 KB
                const size_t lengthOfbuffer = 40 * 1024;
                set_volume( ( this->musicVolume * 255 ) / 100, 0 );
                this->oggStream = alogg_start_streaming( ( isomot::sharePath() + fileName ).c_str (), lengthOfbuffer );
                if ( this->oggStream )
                {
                        if ( loop )
                                this->oggPlayer = alogg_create_thread_which_loops( this->oggStream, ( isomot::sharePath() + fileName ).c_str (), lengthOfbuffer );
                        else
                                this->oggPlayer = alogg_create_thread( this->oggStream );

                        if ( this->oggPlayer )
                                this->oggPlaying = fileName;
                }
        }
}

void SoundManager::stopOgg ()
{
        if ( this->oggPlayer != 0 )
        {
                if ( this->oggPlayer->stop != 1 )
                {
                        alogg_stop_thread( this->oggPlayer );
                        while( alogg_is_thread_alive( this->oggPlayer ) ) ;
                        alogg_join_thread( this->oggPlayer );
                        alogg_destroy_thread( this->oggPlayer );
                }
                this->oggStream = 0;
                this->oggPlayer = 0;
                this->oggPlaying.clear();
        }
}

void SoundManager::setVolumeOfMusic( int volume )
{
          this->musicVolume = volume == 0 ? 1 : volume;
          set_volume( ( this->musicVolume * 255 ) / 100, 0 );
}

SampleData* SoundManager::findSample( short label, const StateId& stateId )
{
        std::list< SoundData >::iterator i = std::find_if( soundData.begin(), soundData.end(), std::bind2nd( EqualSoundData(), label ) );

        return i != soundData.end () ? ( *i ).find( this->translateStateIdToString( stateId ) ) : 0;
}

std::string SoundManager::translateStateIdToString ( const StateId& stateId )
{
        std::string state;

        switch ( stateId )
        {
                case StateWait:
                        state = "wait";
                        break;

                case StatePush:
                        break;

                case StateMove:
                case StateMoveNorth:
                case StateMoveSouth:
                case StateMoveEast:
                case StateMoveWest:
                case StateMoveNortheast:
                case StateMoveSoutheast:
                case StateMoveSouthwest:
                case StateMoveNorthwest:
                case StateMoveUp:
                case StateMoveDown:
                case StateAutoMove:
                case StateAutoMoveNorth:
                case StateAutoMoveSouth:
                case StateAutoMoveEast:
                case StateAutoMoveWest:
                        state = "move";
                        break;

                case StateBlink:
                        break;

                case StateJump:
                case StateRegularJump:
                case StateHighJump:
                        state = "jump";
                        break;

                case StateFall:
                case StateGlide:
                        state = "fall";
                        break;

                case StateTakeItem:
                case StateTakeAndJump:
                        state = "take";
                        break;

                case StateTakenItem:
                break;

                case StateDropItem:
                case StateDropAndJump:
                        state = "drop";
                        break;

                case StateDisplaceNorth:
                case StateDisplaceSouth:
                case StateDisplaceEast:
                case StateDisplaceWest:
                case StateDisplaceNortheast:
                case StateDisplaceSoutheast:
                case StateDisplaceSouthwest:
                case StateDisplaceNorthwest:
                case StateDisplaceUp:
                case StateDisplaceDown:
                        state = "push";
                        break;

                case StateStartWayOutTeletransport:
                case StateWayOutTeletransport:
                        state = "teleport-out";
                        break;

                case StateStartWayInTeletransport:
                case StateWayInTeletransport:
                        state = "teleport-in";
                        break;

                case StateForceDisplaceNorth:
                case StateForceDisplaceSouth:
                case StateForceDisplaceEast:
                case StateForceDisplaceWest:
                        state = "force";
                        break;

                case StateCancelDisplaceNorth:
                case StateCancelDisplaceSouth:
                case StateCancelDisplaceEast:
                case StateCancelDisplaceWest:
                        state = "move";
                        break;

                case StateStartDestroy:
                        break;

                case StateDestroy:
                        state = "destroy";
                        break;

                case StateShot:
                        state = "shot";
                        break;

                case StateRebound:
                        state = "rebound";
                        break;

                case StateSwitch:
                        state = "switch";
                        break;

                case StateCollision:
                        state = "collision";
                        break;

                case StateActive:
                        state = "active";
                        break;

                case StateError:
                        state = "error";
                        break;

                default:
                        ;
        }

        return state;
}

SoundData::SoundData( short label )
        : label( label )
{
        this->path = isomot::sharePath();
}

void SoundData::add( const std::string& state, const std::string& sampleFileName )
{
        SAMPLE* sample = load_sample( ( this->path + sampleFileName ).c_str () );
        assert( sample != 0 );

        SampleData sampleData;
        sampleData.sample = sample;
        sampleData.voice = -1;
        this->table[ state ] = sampleData;
}

SampleData* SoundData::find( const std::string& state )
{
        __gnu_cxx::hash_map< std::string, SampleData >::iterator i = this->table.find( state );

        return i != this->table.end () ? ( &( i->second ) ) : 0;
}

bool EqualSoundData::operator()( const SoundData& soundData, short label ) const
{
        return ( soundData.getLabel() == label );
}
