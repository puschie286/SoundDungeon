// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"

#include "Containers/Map.h"
#include "CustomMeshComponent.h"

#include <limits>

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
			if( Value <= MinValue )
			{
				SetValue( MinValue );
			}
			else if( Value >= MaxValue )
			{
				SetValue( MaxValue );
			}
			else
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
		float Return = MinScale;

		if( Value != std::numeric_limits<float>::infinity() &&
			Value != std::numeric_limits<float>::infinity() * -1 )
		{
			if( bAddScaledValue )
			{
				AddValue( Value );
			}

			float x = 0 - MinCurrent;
			float y = 0 - MinScale;
			Return = ( ( Value + x ) / ( MaxCurrent + x ) ) * ( MaxScale + y ) - y;

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

USTRUCT( BlueprintType )
struct FWaveformConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Waveform Config" )
	UCustomMeshComponent* Component;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Waveform Config" )
	bool DrawChannels;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Waveform Config" )
	bool DrawAsCurve;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Waveform Config" )
	bool DrawFull;

	/**
	* Radius != 0 : Auflösung der Frakgremnte
	* Sonst : Breite der Waveform
	*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Waveform Config" )
	int32 Width;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Waveform Config" )
	int32 Height;
	
	/**
	* Wird als Kreis dargestellt wenn Radius > 0
	*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Waveform Config" )
	int32 Radius;

	FWaveformConfig()
	{
		Component = nullptr;

		DrawChannels = false;
		DrawAsCurve = false;
		DrawFull = false;

		Width = 300;
		Height = 100;
		Radius = 0;
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

	TArray<TArray<float>> ShareStorage;

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

	bool GenerateWaveform( TArray<uint8>* WavPtr, FWaveformConfig* WConfig );

	void CalculateFrequencySpectrum( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray<uint8>* InWavPtr, TArray<TArray<float>> &OutSpectrum );

	void GetAmplitude( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 AmplitudeBuckets, TArray<uint8>* InWavPtr, TArray<TArray<float>>& OutAmplitudes );

	void ClearShareData( const int32 NewSize = 0 );

	void GetShare( TArray<float>& OutData, const int32 Slot = 0 );

	void SetShare( const TArray<float>& InData, const int32 Slot = 0);

	void CalculateWAVData( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, FString WAVName, int32 Slot );

	void GetShareSingle( float& Out, const int32 Slot = 0, const int32 Index = 5 ); // Out == -1 On Faile

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
	static void LIBGenerateWaveform( UPARAM( ref ) TArray<uint8>& InData, UPARAM( ref ) FWaveformConfig& WConfig );
	
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

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBClearShareStorage( const int32 NewSize = 0 );

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBSetShare( UPARAM( ref ) TArray<float>& DataToStore, const int32 Slot = 0 );

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBGetShare( TArray<float>& StoredData, const int32 Slot = 0 );

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBGetShareSingle( float& Out, const int32 Slot = 0, const int32 Index = 5 );

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBArrayMultiply( const TArray<float>& FirstArray, const TArray<float>& SecondArray, TArray<float>& ResultArray );

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBNormalized( const float Value, float &NormalizedValue, const float ScaleMin = 0.f, const float ScaleMax = 1.f, const float Min = 0.f, const float Max = 100.f );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBCalculateWAVData( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, FString WAVName, int32 Slot );

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBUpdateWaveformCurser( UStaticMeshComponent* Target, float Duration, float PlayTime, int32 Radius, FRotator RotOffset, FVector PosOffset );

	UFUNCTION( BlueprintCallable, Category = "Helper" )
	static void LIBUpdateWaveformSelf( UCustomMeshComponent* Target, float Duration, float PlayTime );
};
