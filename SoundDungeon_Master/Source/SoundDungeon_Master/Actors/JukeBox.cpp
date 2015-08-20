#include "SoundDungeon_Master.h"
#include "JukeBox.h"

AJukeBox::AJukeBox( const FObjectInitializer& ObjectInit )
	: Super( ObjectInit )
{
	this->Tags.Add( FName( TEXT( "InteractiveAble" ) ) );

	SoundSource1 = nullptr;
	SoundSource2 = nullptr;
	SoundSource3 = nullptr;
	ErrorSound1 = nullptr;
	ErrorSound2 = nullptr;
	ErrorSound3 = nullptr;
	ErrorSoundSource = nullptr;
	SoundPart1 = nullptr;
	SoundPart2 = nullptr;
	SoundPart3 = nullptr;
	SoundPart4 = nullptr;
	SoundPart5 = nullptr;
	SoundPart6 = nullptr;
	Part1StartTime = 0.f;
	Part2StartTime = 0.f;
	Part3StartTime = 0.f;
	Part4StartTime = 0.f;
	Part5StartTime = 0.f;
	Part6StartTime = 0.f;

	ObjectsState.Init( false, 3 );

	ActionStates = 0;

	PlayerRef = nullptr;
}

void AJukeBox::BeginPlay()
{
	PlayerRef = (APlayerCharacter*)UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 );
	if( !PlayerRef )
	{
		UE_LOG( LogTemp, Warning, TEXT( "No Character found" ) );
	}
	if( !SoundPart1 || !SoundPart2 || !SoundPart3 || !SoundPart4 || !SoundPart5 || !SoundPart6 )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Not all Sounds are set for JukeBox" ) );
		//Repair actions - full reload
		auto LoadSound = []( const TCHAR* Path ) -> USoundWave*
		{
			return Cast<USoundWave>( StaticLoadObject( USoundWave::StaticClass(), NULL, Path ) );
		};
		SoundPart1 = LoadSound( *FString( TEXT( "/Game/AudioFiles/MainVocal.MainVocal" ) ) );
		SoundPart2 = LoadSound( *FString( TEXT( "/Game/AudioFiles/KickDrum.KickDrum" ) ) );
		SoundPart3 = LoadSound( *FString( TEXT( "/Game/AudioFiles/BreakBeat.BreakBeat" ) ) );
		SoundPart4 = LoadSound( *FString( TEXT( "/Game/AudioFiles/Beatz_Flutestep.Beatz_Flutestep" ) ) );
		SoundPart5 = LoadSound( *FString( TEXT( "/Game/AudioFiles/Wobble_Flutestep.Wobble_Flutestep" ) ) );
		SoundPart6 = LoadSound( *FString( TEXT( "/Game/AudioFiles/oriental_Flutestep.oriental_Flutestep" ) ) );
	}
	if( !SoundSource1 || !SoundSource2 || !SoundSource3 || !ErrorSoundSource )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Not all SoundSources are set for JukeBox" ) );
	}
}

void AJukeBox::EnablePower()
{
	if( !ObjectsState[0] )
	{
		ObjectsState[0] = true;
		OnObjectActivation( ActionStates, 0 );
		// Start other insert Sounds

		if( ActionStates )
		{
			SoundSource1->GetAudioComponent()->SetSound( SoundPart4 );
			SoundSource2->GetAudioComponent()->SetSound( SoundPart5 );
			SoundSource3->GetAudioComponent()->SetSound( SoundPart6 );
			SoundSource1->PlaySound( Part4StartTime );
			SoundSource2->PlaySound( Part5StartTime );
			SoundSource3->PlaySound( Part6StartTime );
		}
		else
		{
			SoundSource1->GetAudioComponent()->SetSound( SoundPart1 );
			SoundSource1->PlaySound( Part1StartTime );
			if( ObjectsState[1] )
			{
				SoundSource2->PlaySound( SoundSource1->GetAccPlayTime() );
			}
			if( ObjectsState[2] )
			{
				SoundSource3->PlaySound( SoundSource1->GetAccPlayTime() );
			}
		}
	}
}

void AJukeBox::DropPart( int32 part )
{
	
}

bool AJukeBox::Action( AActor* Target )
{
	if( Target )
	{
		return RoomAction( Target );
	}
	else
	{
		switch( ActionStates )
		{
			case 0:
				return RoomFirstActionActive();
				break;
			case 1:
				return RoomSecondActionActive();
				break;
		}
	}
	return false;
}

bool AJukeBox::RoomAction( AActor* Target )
{
	// TODO : add Fail Sound
	if( Target && PlayerRef )
	{
		for( uint8 i = 0; i < 3; ++i )
		{
			FString ObjectName = FString( TEXT( "Object" ) ) + FString::FromInt( i );
			if( Target->ActorHasTag( FName( *ObjectName ) ) )
			{
				ActivateObject( i );
				PlayerRef->DestroyObject();
				switch( i )
				{
					case 1 :
						SoundSource2->GetAudioComponent()->SetSound( ( ActionStates ) ? ( SoundPart5 ) : ( SoundPart2 ) );
						if( ObjectsState[0] )
						{
							SoundSource2->PlaySound( SoundSource1->GetAccPlayTime() );
						}
						break;
					case 2 :
						SoundSource3->GetAudioComponent()->SetSound( ( ActionStates ) ? ( SoundPart6 ) : ( SoundPart3 ) );
						if( ObjectsState[0] )
						{
							SoundSource3->PlaySound( SoundSource1->GetAccPlayTime() );
						}
						break;
				}
				return true;
			}
		}
	}
	return false;
}

bool AJukeBox::RoomFirstActionActive()
{
	if( ObjectsState.Num() >= 3 )
	{
		if( !ObjectsState[0] )
		{
			ErrorSoundSource->GetAudioComponent()->SetSound( ErrorSound1 );
		}
		else if( !ObjectsState[1] )
		{
			ErrorSoundSource->GetAudioComponent()->SetSound( ErrorSound2 );
		}
		else if( !ObjectsState[2] )
		{
			ErrorSoundSource->GetAudioComponent()->SetSound( ErrorSound3 );
		}
		else // All Objects insert
		{
			ObjectsState.Init( false, 3 );
			OnRoomFinish( ActionStates++ );
			return true;
		}
		ErrorSoundSource->PlaySound();
	}
	return false;
}

bool AJukeBox::RoomSecondActionActive()
{
	if( ObjectsState.Num() >= 3 )
	{
		if( !ObjectsState[0] )
		{
			ErrorSoundSource->GetAudioComponent()->SetSound( ErrorSound1 );
		}
		else if( !ObjectsState[1] )
		{
			ErrorSoundSource->GetAudioComponent()->SetSound( ErrorSound2 );
		}
		else if( !ObjectsState[2] )
		{
			ErrorSoundSource->GetAudioComponent()->SetSound( ErrorSound3 );
		}
		else
		{
			ObjectsState.Init( false, 3 );
			SoundSource1->GetAudioComponent()->SetSound( SoundPart4 );
			SoundSource1->PlaySound( ( SoundSource2->GetAudioComponent()->IsPlaying() ) ? ( SoundSource2->GetAccPlayTime() ) : ( 0.f ) );
			OnRoomFinish( ActionStates++ );
			return true;
		}
		ErrorSoundSource->PlaySound();
	}
	return false;
}

void AJukeBox::ActivateObject( uint8 Object )
{
	if( Object < ObjectsState.Num() )
	{
		if( !ObjectsState[Object] )
		{
			ObjectsState[Object] = true;
			OnObjectActivation( ActionStates, Object );
		}
	}
}
