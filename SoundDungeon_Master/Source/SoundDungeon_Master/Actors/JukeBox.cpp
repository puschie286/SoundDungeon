#include "SoundDungeon_Master.h"
#include "JukeBox.h"

AJukeBox::AJukeBox( const FObjectInitializer& ObjectInit )
	: Super( ObjectInit )
{
	this->Tags.Add( FName( TEXT( "InteractiveAble" ) ) );

	SoundSource = nullptr;
	ErrorSound1 = nullptr;
	ErrorSound2 = nullptr;
	ErrorSound3 = nullptr;

	ActionStates = 0;
}

void AJukeBox::BeginPlay()
{
	if( !SoundSource )
	{
		UE_LOG( LogTemp, Warning, TEXT( "No SoundSource set for JukeBox" ) );
	}
}

bool AJukeBox::Action( AActor* Target )
{
	switch( ActionStates )
	{
		case 0 :
			return ( Target ) ? ( RoomFirstAction( Target ) ) : ( RoomFirstActionActive() );
			break;
		case 1 :
			return ( Target ) ? ( RoomSecondAction( Target ) ) : ( RoomSecondActionActive() );
			break;
	}
	return false;
}

bool AJukeBox::RoomSecondAction( AActor* Target )
{
	return false;
}

bool AJukeBox::RoomFirstAction( AActor* Target )
{
	return false;
}

bool AJukeBox::RoomFirstActionActive()
{
	return false;
}

bool AJukeBox::RoomSecondActionActive()
{
	return false;
}
