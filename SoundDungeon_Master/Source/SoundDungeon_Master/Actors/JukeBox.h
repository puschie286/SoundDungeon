#pragma once

#include "Engine/StaticMeshActor.h"

#include "../Interfaces/SimpleAction.h"

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
	class ASoundSource*		SoundSource;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class USoundWave*		ErrorSound1;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class USoundWave*		ErrorSound2;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class USoundWave*		ErrorSound3;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class USoundWave*		MainSound;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class USoundWave*		SoundPart1;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class USoundWave*		SoundPart2;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class USoundWave*		SoundPart3;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class AActor*			DropPart1;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class AActor*			DropPart2;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	class AActor*			DropPart3;

public : // Functions
	virtual bool	Action( AActor* Target ) override;

	UFUNCTION( BlueprintCallable, Category = "JukeBox" )
	void	EnablePower();

	UFUNCTION( BlueprintCallable, Category = "JukeBox" )
	void	DropPart( int32 part );

	UFUNCTION( BlueprintImplementableEvent, Category = "JukeBox" )
	virtual void	OnObjectActivation( int32 StateID, int32 ObjectID );

	UFUNCTION( BlueprintImplementableEvent, Category = "JukeBox" )
	virtual void	OnRoomFinish( int32 RoomID );

private: // Properties
	TArray<bool> ObjectsState;
	// 0 - Power	- Part0
	// 1 - Disc		- Part1
	// 2 - Coin		- Part3

	uint8	ActionStates;

	class APlayerCharacter* PlayerRef;

private: // Functions
	bool	RoomAction( AActor* Target );

	bool	RoomFirstActionActive();
	bool	RoomSecondActionActive();

	void	ActivateObject( uint8 Object );
};
