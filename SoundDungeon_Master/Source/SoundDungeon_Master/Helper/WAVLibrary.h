// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"

#include "Containers/Map.h"
#include "CustomMeshComponent.h"

#include "WAVLibrary.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = ( Custom ), Meta = ( BlueprintSpawnableComponent ) )
class SOUNDDUNGEON_MASTER_API UWAVLibrary : public UObject
{
	GENERATED_BODY()
	
private:
	UWAVLibrary();

	static UWAVLibrary* Instance;
	TMap<FName, TPair<uint8, TArray<uint8>*>> DataStorage;

	FString Path;

	bool CheckFName( FName Name );

	bool CheckStorageContain( FName Name );

public:

	static UWAVLibrary* GetInstance();

	bool LoadWAV( FName WAVName );

	bool UnloadWAV( FName WAVName );

	bool IsLoadedWAV( FName WAVName );

	TArray<uint8>* GetWAV( FName WAVName );

	bool FinishedUsage( FName WAVName );

	bool FinishedUsage( TArray<uint8>* WavPtr );

	bool GenerateWaveform( TArray<uint8>* WavPtr, UCustomMeshComponent* InComponent, bool DrawChannels = false, bool DrawAsCurve = false, uint32 Width = 300, uint32 Height = 100 );

	//void CalculateFrequencySpectrum( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray<uint8>* InWavPtr, TArray<TArray<float>> &OutSpectrum );

	void GetAmplitude( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 AmplitudeBuckets, TArray<uint8>* InWavPtr, TArray<TArray<float>>& OutAmplitudes );
	
	bool bUseLog;

	bool bUnloadWhenNotUsed;

	// Blueprint functions
	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBLoadWAV( FString WAVName );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBUnloadWAV( FString WAVName );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBFinishedUsage( FString WAVName );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBGetWAV( FString WAVName, TArray<uint8>& OutData, bool Force = false );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBGenerateWaveform( UPARAM( ref ) TArray<uint8>& InData, UCustomMeshComponent* InComponent, bool DrawChannels = false, bool DrawAsCurve = false, int32 Width = 300, int32 Height = 100 );

	/** Calculates the frequency spectrum for a window of time for the SoundWave
	* @param InData - The waveData to generate the spectrum for
	* @param Channel - The channel of the sound to calculate.  Specify 0 to combine channels together
	* @param StartTime - The beginning of the window to calculate the spectrum of
	* @param TimeLength - The duration of the window to calculate the spectrum of
	* @param SpectrumWidth - How wide the spectrum is.  The total samples in the window are divided evenly across the spectrum width.
	* @return OutSpectrum - The resulting spectrum
	*/
	/*
	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBCalculateFrequencySpectrum( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, UPARAM( ref ) TArray<uint8>& InData, TArray<float> &OutSpectrum );
	*/
	/** Gathers the amplitude of the wave data for a window of time for the SoundWave
	* @param InData - The waveData to get samples from
	* @param Channel - The channel of the sound to get.  Specify 0 to combine channels together
	* @param StartTime - The beginning of the window to get the amplitude from
	* @param TimeLength - The duration of the window to get the amplitude from
	* @param AmplitudeBuckets - How many samples to divide the data in to.  The amplitude is averaged from the wave samples for each bucket
	* @return OutAmplitudes - The resulting amplitudes
	*/
	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBGetAmplitude( int32 Channel, float StartTime, float TimeLength, int32 AmplitudeBuckets, UPARAM( ref ) TArray<uint8>& InData, TArray<float> &OutAmplitudes );
};
