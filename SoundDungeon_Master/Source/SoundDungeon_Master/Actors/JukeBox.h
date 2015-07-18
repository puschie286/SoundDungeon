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

	UFUNCTION( BlueprintCallable, Category = "JukeBox" )
	void	EnablePower();

	UFUNCTION( BlueprintImplementableEvent, Category = "JukeBox" )
	virtual void	OnObjectActivation( int32 StateID, int32 ObjectID );

private: // Properties
	TArray<bool> ObjectsState;
	// 0 - Power	- Part0
	// 1 - Disc		- Part1
	// 2 - Coin		- Part3

	uint8	ActionStates;

private: // Functions
	bool	RoomFirstAction( AActor* Target );
	bool	RoomSecondAction( AActor* Target );

	bool	RoomFirstActionActive();
	bool	RoomSecondActionActive();

	void	ActivateObject( uint8 Object );
};
