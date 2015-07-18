#pragma once

#include "Engine/StaticMeshActor.h"

#include "../Interfaces/SimpleAction.h"

#include "SoundSource.h"
#include "Sound/SoundWave.h"

#include "JukeBox.generated.h"


/**
 * 
 */
UCLASS()
class SOUNDDUNGEON_MASTER_API AJukeBox : public AStaticMeshActor, public ISimpleAction
{
	GENERATED_BODY()
	
public: // Unreal Functions & Construct
	AJukeBox( const FObjectInitializer& ObjectInit );
	
	void BeginPlay();
public: // Properties
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	ASoundSource*	SoundSource;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		ErrorSound1;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		ErrorSound2;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		ErrorSound3;

public : // Functions
	virtual bool	Action( AActor* Target ) override;

private: // Properties
	bool	Object0;
	bool	Object1;
	bool	Object2;

	uint8	ActionStates;

private: // Functions
	bool	RoomFirstAction( AActor* Target );
	bool	RoomSecondAction( AActor* Target );

	bool	RoomFirstActionActive();
	bool	RoomSecondActionActive();
};
