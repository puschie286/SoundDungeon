#include "SoundDungeon_Master.h"
#include "SoundSource.h"
#include "Components/AudioComponent.h"

ASoundSource::ASoundSource( const FObjectInitializer &ObjectInitializer )
	: Super( ObjectInitializer )
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	PlayTime = 0.f;
	StartStamp = 0.0;
	LastUpdateStamp = 0.0;
	Duration = 0.f;
}

void ASoundSource::PlaySound( float StartTime /*= 0.f */ )
{
	if( GetAudioComponent() && GetAudioComponent()->Sound )
	{
		Duration = ( (USoundWave*)( GetAudioComponent()->Sound ) )->Duration;
		PlayTime = FMath::Max( 0.f, StartTime );
		while( PlayTime > Duration )
		{
			PlayTime -= Duration;
		}
		StartStamp = FPlatformTime::Seconds() - PlayTime;
		LastUpdateStamp = StartStamp;

		GetAudioComponent()->Play( PlayTime );

		if( StartTime <= 0.f )
		{
			OnSoundStart();
		}
		OnSoundPlay();
	}
}

void ASoundSource::StopSound()
{
	GetAudioComponent()->Stop();

	StartStamp = 0.0;
	LastUpdateStamp = 0.0;
	PlayTime = 0.f;

	OnSoundStop();
}

void ASoundSource::PauseSound()
{
	GetAudioComponent()->Stop();

	LastUpdateStamp = FPlatformTime::Seconds();
	
	OnSoundPause();
}

void ASoundSource::UnpauseSound()
{
	GetAudioComponent()->Play( PlayTime );

	if( LastUpdateStamp > 0 )
	{
		StartStamp += FPlatformTime::Seconds() - LastUpdateStamp;
	}

	OnSoundUnpause();
}

void ASoundSource::GetSoundString( FString &SoundName )
{
	if( GetAudioComponent() )
	{
		int32 IndexFound = 0;
		GetAudioComponent()->GetDetailedInfo().FindLastChar( '.', IndexFound );
		SoundName = GetAudioComponent()->GetDetailedInfo().RightChop( IndexFound + 1 );
	}
}

void ASoundSource::GetAccurratPlayTime( float& PlaybackTime )
{
	PlaybackTime = (float)( FPlatformTime::Seconds() - StartStamp );
}

float ASoundSource::GetAccPlayTime()
{
	return (float)( FPlatformTime::Seconds() - StartStamp );
}

void ASoundSource::SyncTimer()
{
	if( GetAudioComponent()->IsPlaying() )
	{
		LastUpdateStamp = FPlatformTime::Seconds();
		PlayTime = LastUpdateStamp - StartStamp;
	}
}

void ASoundSource::HardSync()
{
	SyncTimer();
	if( GetAudioComponent()->IsPlaying() )
	{
		PlaySound( PlayTime );
	}
}

void ASoundSource::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	SyncTimer();

	if( GetAudioComponent()->IsPlaying() && PlayTime + DeltaTime >= Duration )
	{
		OnSoundEnd();
	}
}

// ExportBUG Workaround
#if WITH_EDITOR
void ASoundSource::CheckForErrors( void )
{

}

bool ASoundSource::GetReferencedContentObjects( TArray<UObject*>& Objects ) const
{
	if( GetAudioComponent()->Sound )
	{
		Objects.Add( GetAudioComponent()->Sound );
	}
	return true;
}

#endif

void ASoundSource::PostRegisterAllComponents()
{
#if WITH_EDITORONLY_DATA
	if( GetAudioComponent() && bHiddenEdLevel )
	{
		GetAudioComponent()->Stop();
	}
#endif
}
