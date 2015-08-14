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
	MainSound = nullptr;
	SoundPart1 = nullptr;
	SoundPart2 = nullptr;
	SoundPart3 = nullptr;
	SoundPart4 = nullptr;
	SoundPart5 = nullptr;
	SoundPart6 = nullptr;
	Part1StartTime = 0.f;
	Part2StartTime = 0.f;
	Part3StartTime = 0.f;

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
	if( !SoundSource1 || !SoundSource2 || !SoundSource3 || !ErrorSoundSource )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Not all SoundSources are set for JukeBox" ) );
	}
	if( !DropPart1 || !DropPart2 || !DropPart3 )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Not all DropParts are set correct" ) );
	}
	else
	{
		DropPart1->SetActorHiddenInGame( true );
		DropPart2->SetActorHiddenInGame( true );
		DropPart3->SetActorHiddenInGame( true );
	}
}

void AJukeBox::EnablePower()
{
	ActivateObject( 0 );
}

void AJukeBox::DropPart( int32 part )
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z -= GetActorScale().Z * 10;
	AActor* Target = nullptr;
	switch( part )
	{
		case 0 :
			Target = DropPart1;
			break;
		case 1 :
			Target = DropPart2;
			break;
		case 2 :
			Target = DropPart3;
			break;
		default:
			UE_LOG( LogTemp, Warning, TEXT( "DropPart %i not found" ), part );
			break;
	}
	if( Target )
	{
		if( Target->SetActorLocation( NewLocation ) )
		{
			Target->SetActorHiddenInGame( false );
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "Cant set Location of Part-Actor" ) );
		}
	}
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
				if( i == 2 )
				{
					if( ActionStates == 0 )
					{
						return RoomFirstActionActive();
					}
					else if( ActionStates == 1 )
					{
						return RoomSecondActionActive();
					}
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
			SoundSource1->GetAudioComponent()->SetSound( SoundPart1 );
			SoundSource2->GetAudioComponent()->SetSound( SoundPart2 );
			SoundSource3->GetAudioComponent()->SetSound( SoundPart3 );
			SoundSource1->PlaySound( Part1StartTime );
			SoundSource2->PlaySound( Part2StartTime );
			SoundSource3->PlaySound( Part3StartTime );
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
			SoundSource1->GetAudioComponent()->SetSound( SoundPart4 );
			SoundSource2->GetAudioComponent()->SetSound( SoundPart5 );
			SoundSource3->GetAudioComponent()->SetSound( SoundPart6 );
			SoundSource1->PlaySound( Part1StartTime );
			SoundSource2->PlaySound( Part2StartTime );
			SoundSource3->PlaySound( Part3StartTime );
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
