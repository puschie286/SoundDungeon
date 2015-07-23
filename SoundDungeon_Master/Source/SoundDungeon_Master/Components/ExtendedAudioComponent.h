// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/AudioComponent.h"

#include "SoundDefinitions.h"

#include "ExtendedAudioComponent.generated.h"
/**
 * 
 */
UCLASS( ClassGroup = ( Custom ), Meta = ( BlueprintSpawnableComponent ) )
class SOUNDDUNGEON_MASTER_API UExtendedAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	
	UExtendedAudioComponent( const FObjectInitializer& ObjectInitializer );
	
	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void PlaySound( float StartTime = 0.f );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void StopSound();

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void PauseSound();

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void UnpauseSound();

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "SoundVisualize" )
	float PlayTime;

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundVisualize" )
	void OnSoundPlay();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundVisualize" )
	void OnSoundStop();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundVisualize" )
	void OnSoundEnd();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundVisualize" )
	void OnSoundPause();

	UFUNCTION( BlueprintImplementableEvent, Category = "SoundVisualize" )
	void OnSoundUnpause();

	void TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction );
};
