// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/AudioComponent.h"

#include "SoundDefinitions.h"

#include "VisualizableAudioComponent.generated.h"
/**
 * 
 */
UCLASS()
class SOUNDDUNGEON_MASTER_API UVisualizableAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	
	UVisualizableAudioComponent( const FObjectInitializer& ObjectInitializer );

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "SoundVisualize" )
	bool UseSoundName;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "SoundVisualize" )
	FString DataName;
	
	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void LoadData( FString FileName = "None" );
	
	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void PlaySound( float StartTime = 0.f );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void StopSound();

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void PauseSound();

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void UnpauseSound();

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void CalculateFrequencySpectrum( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, TArray<float> &OutSpectrum );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	void GetAmplitude( int32 Channel, float StartTime, float TimeLength, int32 AmplitudeBuckets, TArray<float> &OutAmplitudes );

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "SoundVisualize" )
	bool IsLoaded;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "SoundVisualize" )
	float PlayTime;

	void TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction );

	void CalculateFrequencySpectrum( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray<TArray<float>> &OutSpectrum );

	void GetAmplitude( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 AmplitudeBuckets, TArray<TArray<float>>& OutAmplitudes );

protected:

	class USoundWave* SoundWave;

	TArray<uint8> wavData;
};
