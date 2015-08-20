#pragma once

#include "Engine/StaticMeshActor.h"

#include "../Interfaces/SimpleAction.h"

#include "JukeBox.generated.h"

class ASoundSource;
class USoundWave;
class AActor;

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
	ASoundSource*		SoundSource1;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	ASoundSource*		SoundSource2;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	ASoundSource*		SoundSource3;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	ASoundSource*		ErrorSoundSource;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		ErrorSound1;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		ErrorSound2;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		ErrorSound3;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		SoundPart1;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		SoundPart2;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		SoundPart3;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		SoundPart4;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		SoundPart5;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	USoundWave*		SoundPart6;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	float			Part1StartTime;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	float			Part2StartTime;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	float			Part3StartTime;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	float			Part4StartTime;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	float			Part5StartTime;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Default )
	float			Part6StartTime;

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
