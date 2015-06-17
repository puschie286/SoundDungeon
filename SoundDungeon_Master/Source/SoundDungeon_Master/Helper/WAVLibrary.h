// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"

#include "Containers/Map.h"
#include "CustomMeshComponent.h"

#include "WAVLibrary.generated.h"

USTRUCT( BlueprintType )
struct FScaling
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	float MinValue;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	float MaxValue;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	float MinScale;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	float MaxScale;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	float MinCurrent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	float MaxCurrent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	bool bUseMinMaxValue;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	bool bAddScaledValue; // AddValue before GetScaled for Value

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	uint8 InterpolateInterval; // 0 for no Interpolation
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Scaling" )
	bool bInterpolateIgnoreNull;

	UPROPERTY()
	uint8 InterpolateCounter;

	UPROPERTY()
	TArray<float> InterpolateStorage;

	void AddValue( const float Value )
	{
		auto SetValue = [&]( const float Value ) -> void
		{
			if( Value > MaxCurrent )
			{
				MaxCurrent = Value;
			}
			else if( Value < MinCurrent )
			{
				MinCurrent = Value;
			}
		};

		if( bUseMinMaxValue )
		{
			if( Value >= MinValue && Value <= MaxValue )
			{
				SetValue( Value );
			}
		}
		else
		{
			SetValue( Value );
		}
	}

	float GetScaled( const float Value )
	{
		if( bAddScaledValue )
		{
			AddValue( Value );
		}

		float x = 0 - MinCurrent;
		float y = 0 - MinScale;
		float Return = ( ( Value + x ) / ( MaxCurrent + x ) ) * ( MaxScale + y ) - y;

		if( InterpolateInterval != 0 ) // Interpolate Scaled Value
		{
			if( InterpolateStorage.Num() != InterpolateInterval ) // Resize Storage
			{
				InterpolateStorage.SetNum( InterpolateInterval );
				float SetValue = ( bInterpolateIgnoreNull && Return == 0.f ) ? ( 0.01f ) : ( Return );
				for( float& Element : InterpolateStorage )
				{
					Element = SetValue;
				}
				InterpolateCounter = 0;
			}
			else // Calculate Average
			{
				float Average = 0.f;
				float SetValue = Return;

				if( bInterpolateIgnoreNull && Return == 0.f ) // Interpolate Ignore
				{
					if( 0.01f > MaxScale )
					{
						SetValue = MaxScale;
					}
					else if( 0.01f < MinScale )
					{
						SetValue = MinScale;
					}
					else
					{
						SetValue = 0.01f;
					}
				}
				InterpolateStorage[InterpolateCounter] = SetValue;

				for( const float Element : InterpolateStorage )
				{
					Average += Element;
				}
				Return = Average / InterpolateInterval;
			}
			
			if( InterpolateCounter + 1 >= InterpolateInterval ) // Counter Switch
			{
				InterpolateCounter = 0;
			}
			else
			{
				InterpolateCounter++;
			}
		}
		return Return;
	}

	void GetScaled( const float Value, float& ScaledValue )
	{
		ScaledValue = GetScaled( Value );
	}

	FScaling()
	{
		InterpolateInterval = 0;
		InterpolateCounter = 0;
		InterpolateStorage.Empty();
		bInterpolateIgnoreNull = true;

		MinScale = 0.f;
		MaxScale = 1.f;

		MinValue = -100.f;
		MaxValue = 100.f;

		MinCurrent = 0.f;
		MaxCurrent = 0.f;

		bUseMinMaxValue = false;
		bAddScaledValue = true;
	}
};

UCLASS( ClassGroup = ( Custom ), Meta = ( BlueprintSpawnableComponent ) )
class SOUNDDUNGEON_MASTER_API UWAVLibrary : public UObject
{
	GENERATED_BODY()

private:
	UWAVLibrary();

	static UWAVLibrary* Instance;
	
	TMap<FName, TPair<uint8, TArray<uint8>*>> DataStorage;

	FString Path;

	TArray<float>* ShareData;

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

	void CalculateFrequencySpectrum( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray<uint8>* InWavPtr, TArray<TArray<float>> &OutSpectrum );

	void GetAmplitude( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 AmplitudeBuckets, TArray<uint8>* InWavPtr, TArray<TArray<float>>& OutAmplitudes );

	void GetShare( TArray<float> *SharedData );

	void SetShare( TArray<float> *SharedData );

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
	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBCalculateFrequencySpectrum( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, UPARAM( ref ) TArray<uint8>& InData, TArray<float> &OutSpectrum );
	
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

	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	static void LIBScaleAddValue( UPARAM( ref ) FScaling& Scale, const float Value );

	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	static void LIBScaleGetScaled( UPARAM( ref ) FScaling& Scale, const float Value, float& ScaledValue );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBSetShare( UPARAM( ref ) TArray<float>& DataToStore );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBGetShare( TArray<float>& StoredData );

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBArrayMultiply( const TArray<float>& FirstArray, const TArray<float>& SecondArray, TArray<float>& ResultArray );
};
