#include "SoundDungeon_Master.h"
#include "SoundSource.h"
#include "Components/AudioComponent.h"

ASoundSource::ASoundSource( const FObjectInitializer &ObjectInitializer )
	: Super( ObjectInitializer )
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	PlayTime = 0.f;
}

void ASoundSource::PlaySound( float StartTime /*= 0.f */ )
{
	GetAudioComponent()->Play( StartTime );
	PlayTime = StartTime;
	if( StartTime == 0.f )
	{
		OnSoundStart();
	}
	OnSoundPlay();
}

void ASoundSource::StopSound()
{
	GetAudioComponent()->Stop();
	PlayTime = 0.f;
	OnSoundStop();
}

void ASoundSource::PauseSound()
{
	GetAudioComponent()->Stop();
	OnSoundPause();
}

void ASoundSource::UnpauseSound()
{
	GetAudioComponent()->Play( PlayTime );
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

void ASoundSource::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( GetAudioComponent()->IsPlaying() )
	{
		PlayTime += DeltaTime;
		if( PlayTime >= ( (USoundWave*)( GetAudioComponent()->Sound ) )->Duration )
		{
			OnSoundEnd();
		}
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
