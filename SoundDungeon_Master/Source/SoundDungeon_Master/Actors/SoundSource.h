#pragma once

#include "Sound/AmbientSound.h"

#include "SoundSource.generated.h"

/**
 * 
 */
UCLASS()
class SOUNDDUNGEON_MASTER_API ASoundSource : public AAmbientSound
{
	GENERATED_BODY()

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "SoundSource" )
	float PlayTime;

public:
	ASoundSource( const FObjectInitializer &ObjectInitializer );

	UFUNCTION( BlueprintCallable, Category = "SoundSource" )
	void PlaySound( float StartTime = 0.f );
	
	UFUNCTION( BlueprintCallable, Category = "SoundSource" )
	void StopSound();

	UFUNCTION( BlueprintCallable, Category = "SoundSource" )
	void PauseSound();

	UFUNCTION( BlueprintCallable, Category = "SoundSource" )
	void UnpauseSound();
	
	UFUNCTION( BlueprintPure, Category = "SoundSource" )
	void GetSoundString( FString &SoundName );

	UFUNCTION( BlueprintCallable, Category = "SoundSource" )
	void GetAccurratPlayTime( float& PlaybackTime );

	UFUNCTION( BlueprintCallable, Category = "SoundSource" )
	void SyncTimer();

	UFUNCTION( BlueprintCallable, Category = "SoundSource" )
	void HardSync();

	void Tick( float DeltaTime );
	// Events

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundSource" )
	virtual void OnSoundPlay();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundSource" )
	virtual void OnSoundStop();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundSource" )
	virtual void OnSoundPause();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundSource" )
	virtual void OnSoundUnpause();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundSource" )
	virtual void OnSoundStart();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundSource" )
	virtual void OnSoundEnd();

	// ExportBUG Workaround
	void CheckForErrors( void );

	bool GetReferencedContentObjects( TArray<UObject*>& Objects ) const;

	void PostRegisterAllComponents();
private:

	double StartStamp;
	double LastUpdateStamp;
	float Duration;
};
