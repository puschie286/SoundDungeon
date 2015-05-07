// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "VisualizableAudioComponent.h"

#include "tools/kiss_fftnd.h"

// Helper Func
float GetFFTInValue( const int16 SampleValue, const int16 SampleIndex, const int16 SampleCount )
{
	// Apply the Hann window
	return SampleValue * ( 0.5f * ( 1 - FMath::Cos( 2 * PI * SampleIndex / ( SampleCount - 1 ) ) ) );
}

UVisualizableAudioComponent::UVisualizableAudioComponent( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	PrimaryComponentTick.bCanEverTick = true;

	DataName = "MainVocal.wav";

	UseSoundName = true;

	IsLoaded = false;

	PlayTime = 0.f;
}

void UVisualizableAudioComponent::LoadData( FString FileName )
{
	FString Path = FPaths::GameDir() + "Content\\AudioFiles\\";

	if( SoundWave )
	{
		if( FileName.IsEmpty() || FileName == "None" || FileName == "none" )
		{
			if( UseSoundName )
			{
				Path += SoundWave->GetName();
			}
			else
			{
				Path += DataName;
			}
		}
		else
		{
			Path += FileName;
		}

		if( !Path.Contains( ".wav" ) )
		{
			Path += ".wav";
		}

		bool FileLoaded = FFileHelper::LoadFileToArray( wavData, Path.GetCharArray().GetData() );
		
		if( FileLoaded )
		{
			SoundWave = ( USoundWave* )this->Sound;
			IsLoaded = true;
			UE_LOG( LogTemp, Log, TEXT( "AudioData loading successfull : %s" ), *( Path ) );
		}
		else
		{
			wavData.Empty();
			UE_LOG( LogTemp, Warning, TEXT( "AudioData loading failed : %s " ), *( Path ) );
		}
	} 
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "No Sound selected" ) );
	}
}

void UVisualizableAudioComponent::CalculateFrequencySpectrum( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, TArray<float> &OutSpectrum )
{
	OutSpectrum.Empty();

	if( IsLoaded )
	{
		if( SpectrumWidth <= 0 )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Invalid SpectrumWidth %d" ), SpectrumWidth );
		}
		else if( Channel < 0 )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Invalid Channel %d" ), Channel );
		}
		else
		{
			if( StartTime <= 0 )
			{
				UE_LOG( LogTemp, Log, TEXT( " Invalid StartTime %d - adjusting..." ), StartTime );
				StartTime = 0.01f;
			}
			TArray<TArray<float>> Spectrums;

			CalculateFrequencySpectrum( ( Channel != 0 ), StartTime, TimeLength, SpectrumWidth, Spectrums );

			if( Channel == 0 )
			{
				OutSpectrum = Spectrums[0];
			}
			else if( Channel < Spectrums.Num() )
			{
				OutSpectrum = Spectrums[Channel - 1];
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "Requested channel %d, only support sound with 1 or 2 channels" ), Channel );
			}
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "No AudioData loaded" ) );
	}
}

void UVisualizableAudioComponent::GetAmplitude( int32 Channel, float StartTime, float TimeLength, int32 AmplitudeBuckets, TArray<float> &OutAmplitudes )
{
	OutAmplitudes.Empty();

	if( IsLoaded )
	{
		if( Channel >= 0 && AmplitudeBuckets > 0 )
		{
			if( StartTime <= 0 )
			{
				UE_LOG( LogTemp, Log, TEXT( " Invalid StartTime %d - adjusting..." ) );
				StartTime = 0.01f;
			}

			TArray<TArray<float>> Amplitudes;

			GetAmplitude( ( Channel != 0 ), StartTime, TimeLength, AmplitudeBuckets, Amplitudes );

			if( Channel == 0 )
			{
				OutAmplitudes = Amplitudes[0];
			}
			else if( Channel <= Amplitudes.Num() )
			{
				OutAmplitudes = Amplitudes[Channel - 1];
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "Requested channel %d, only support sound with 1 or 2 channels" ), Channel );
			}
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "Invalid Channel %d or AmplitudeBuckets <= 0" ), Channel );
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "No AudioData loaded" ) );
	}
}

void UVisualizableAudioComponent::PlaySound( float StartTime )
{
	if( SoundWave )
	{
		if( StartTime >= 0.f && StartTime < SoundWave->Duration )
		{
			this->PlayTime = StartTime;
			this->Play( StartTime );
		}
	}
}

void UVisualizableAudioComponent::StopSound()
{
	if( SoundWave )
	{
		this->PlayTime = 0.f;
		this->Stop();
	}
}

void UVisualizableAudioComponent::PauseSound()
{
	if( SoundWave )
	{
		this->Stop();
	}
}

void UVisualizableAudioComponent::UnpauseSound()
{
	if( SoundWave )
	{
		if( this->PlayTime >= 0.f && this->PlayTime < SoundWave->Duration )
		{
			this->Play( this->PlayTime );
		}
	}
}

void UVisualizableAudioComponent::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( this->IsPlaying() )
	{
		this->PlayTime += DeltaTime;
	}
}

void UVisualizableAudioComponent::CalculateFrequencySpectrum( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray<TArray<float>> &OutSpectrum )
{
	OutSpectrum.Empty();

	if( IsLoaded )
	{
		const int32 NumChannels = SoundWave->NumChannels;

		if( SpectrumWidth > 0 && NumChannels > 0 )
		{
			// Setup Output
			OutSpectrum.AddZeroed( ( bSplitChannels ? NumChannels : 1 ) );

			for( int32 ChannelIndex = 0; ChannelIndex < OutSpectrum.Num(); ++ChannelIndex )
			{
				OutSpectrum[ChannelIndex].AddZeroed( SpectrumWidth );
			}

			if( wavData.Num() > 0 )
			{
				// Lock RAW Data
				uint8 *RawWaveData = wavData.GetData();
				int32 RawDataSize = wavData.Num();
				FWaveModInfo WaveInfo;

				// Parse WaveData
				if( WaveInfo.ReadWaveHeader( RawWaveData, RawDataSize, 0 ) )
				{
					uint32 SampleCount = 0;
					uint32 SampleCounts[10] = { 0 };

					uint32 FirstSample = *WaveInfo.pSamplesPerSec * StartTime;
					uint32 LastSample = *WaveInfo.pSamplesPerSec * ( StartTime + TimeLength );

					if( NumChannels <= 2 )
					{
						SampleCount = WaveInfo.SampleDataSize / ( 2 * NumChannels );
					}
					else
					{
						UE_LOG( LogTemp, Log, TEXT( "WARNING, Frequency : Not supported in packed" ) );
#if WITH_EDITORONLY_DATA
						for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
						{
							SampleCounts[ChannelIndex] = ( SoundWave->ChannelSizes[ChannelIndex] / 2 );
							SampleCount = FMath::Max( SampleCount, SampleCounts[ChannelIndex] );
							SampleCounts[ChannelIndex] -= FirstSample;
						}
#endif
					}

					FirstSample = FMath::Min( SampleCount, FirstSample );
					LastSample = FMath::Min( SampleCount, LastSample );

					int32 SamplesToRead = LastSample - FirstSample;

					if( SamplesToRead > 0 )
					{
						// Shift the window enough so that we get a power of 2
						int32 PoT = 2;
						while( SamplesToRead > PoT ) PoT *= 2;
						FirstSample = FMath::Max( (unsigned int)0, FirstSample - ( PoT - SamplesToRead ) / 2 );
						SamplesToRead = PoT;
						LastSample = FirstSample + SamplesToRead;
						if( LastSample > SampleCount )
						{
							FirstSample = LastSample - SamplesToRead;
						}
						if( FirstSample < 0 )
						{
							// If we get to this point we can't create a reasonable window so just give up
							return;
						}

						kiss_fft_cpx* buf[10] = { 0 };
						kiss_fft_cpx* out[10] = { 0 };

						int32 Dims[1] = { SamplesToRead };
						kiss_fftnd_cfg stf = kiss_fftnd_alloc( Dims, 1, 0, NULL, NULL );


						int16* SamplePtr = reinterpret_cast<int16*>( WaveInfo.SampleDataStart );
						if( NumChannels <= 2 )
						{
							for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
							{
								buf[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC( sizeof( kiss_fft_cpx ) * SamplesToRead );
								out[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC( sizeof( kiss_fft_cpx ) * SamplesToRead );
							}

							SamplePtr += ( FirstSample * NumChannels );

							for( int32 SampleIndex = 0; SampleIndex < SamplesToRead; ++SampleIndex )
							{
								for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
								{
									buf[ChannelIndex][SampleIndex].r = GetFFTInValue( *SamplePtr, SampleIndex, SamplesToRead );
									buf[ChannelIndex][SampleIndex].i = 0.f;

									SamplePtr++;
								}
							}
						}
						else
						{
							UE_LOG( LogTemp, Log, TEXT( "WARNING, Frequency : Not supported in packed" ) );
#if WITH_EDITORONLY_DATA
							for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
							{
								// Drop this channel out if there isn't the power of 2 number of samples available
								if( SampleCounts[ChannelIndex] >= (uint32)SamplesToRead )
								{
									buf[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC( sizeof( kiss_fft_cpx ) * SamplesToRead );
									out[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC( sizeof( kiss_fft_cpx ) * SamplesToRead );

									for( int32 SampleIndex = 0; SampleIndex < SamplesToRead; ++SampleIndex )
									{
										buf[ChannelIndex][SampleIndex].r = GetFFTInValue( *( SamplePtr + FirstSample + SampleIndex + SoundWave->ChannelOffsets[ChannelIndex] / 2 ), SampleIndex, SamplesToRead );
										buf[ChannelIndex][SampleIndex].i = 0.f;
									}
								}
							}
#endif
						}

						for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
						{
							if( buf[ChannelIndex] )
							{
								kiss_fftnd( stf, buf[ChannelIndex], out[ChannelIndex] );
							}
						}

						int32 SamplesPerSpectrum = SamplesToRead / ( 2 * SpectrumWidth );
						int32 ExcessSamples = SamplesToRead % ( 2 * SpectrumWidth );

						int32 FirstSampleForSpectrum = 1;
						for( int32 SpectrumIndex = 0; SpectrumIndex < SpectrumWidth; ++SpectrumIndex )
						{
							static bool doLog = false;

							int32 SamplesRead = 0;
							double SampleSum = 0;
							int32 SamplesForSpectrum = SamplesPerSpectrum + ( ExcessSamples-- > 0 ? 1 : 0 );
							if( doLog ) UE_LOG( LogTemp, Log, TEXT( "----" ) );
							for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
							{
								if( out[ChannelIndex] )
								{
									if( bSplitChannels )
									{
										SampleSum = 0;
									}

									for( int32 SampleIndex = 0; SampleIndex < SamplesForSpectrum; ++SampleIndex )
									{
										float PostScaledR = out[ChannelIndex][FirstSampleForSpectrum + SampleIndex].r * 2.f / SamplesToRead;
										float PostScaledI = out[ChannelIndex][FirstSampleForSpectrum + SampleIndex].i * 2.f / SamplesToRead;
										//float Val = FMath::Sqrt(FMath::Square(PostScaledR) + FMath::Square(PostScaledI));
										float Val = 10.f * FMath::LogX( 10.f, FMath::Square( PostScaledR ) + FMath::Square( PostScaledI ) );
										if( doLog ) UE_LOG( LogTemp, Log, TEXT( "%.2f" ), Val );
										SampleSum += Val;
									}

									if( bSplitChannels )
									{
										OutSpectrum[ChannelIndex][SpectrumIndex] = (float)( SampleSum / SamplesForSpectrum );
									}
									SamplesRead += SamplesForSpectrum;
								}
							}

							if( !bSplitChannels )
							{
								OutSpectrum[0][SpectrumIndex] = (float)( SampleSum / SamplesRead );
							}

							FirstSampleForSpectrum += SamplesForSpectrum;
						}

						KISS_FFT_FREE( stf );
						for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
						{
							if( buf[ChannelIndex] )
							{
								KISS_FFT_FREE( buf[ChannelIndex] );
								KISS_FFT_FREE( out[ChannelIndex] );
							}
						}
					}
				}
				else
				{
					UE_LOG( LogTemp, Warning, TEXT( "WaveHeader not found" ) );
				}
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "No BulkData found " ) );
			}
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "SpectrumWidth or NumChannels <= 0" ) );
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "No AudioData loaded" ) );
	}
}

void UVisualizableAudioComponent::GetAmplitude( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 AmplitudeBuckets, TArray<TArray<float>>& OutAmplitudes )
{
	OutAmplitudes.Empty();

	if( IsLoaded )
	{
		const int32 NumChannels = SoundWave->NumChannels;

		if( AmplitudeBuckets > 0 && NumChannels > 0 )
		{
			// Setup Output
			OutAmplitudes.AddZeroed( ( bSplitChannels ? NumChannels : 1 ) );

			for( int32 ChannelIndex = 0; ChannelIndex < OutAmplitudes.Num(); ++ChannelIndex )
			{
				OutAmplitudes[ChannelIndex].AddZeroed( AmplitudeBuckets );
			}

			if( wavData.Num() > 0 )
			{
				uint8 *RawWaveData = wavData.GetData();
				int32 RawDataSize = wavData.Num();

				FWaveModInfo WaveInfo;

				// Parse WaveData
				if( WaveInfo.ReadWaveHeader( RawWaveData, RawDataSize, 0 ) )
				{
					uint32 SampleCount = 0;
					uint32 SampleCounts[10] = { 0 };

					uint32 FirstSample = *WaveInfo.pSamplesPerSec * StartTime;
					uint32 LastSample = *WaveInfo.pSamplesPerSec * ( StartTime + TimeLength );

					if( NumChannels <= 2 )
					{
						SampleCount = WaveInfo.SampleDataSize / ( 2 * NumChannels );
					}
					else
					{
						UE_LOG( LogTemp, Log, TEXT( "WARNING, Amplitude : Not supported in packed" ) );
#if WITH_EDITORONLY_DATA
						for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
						{
							SampleCounts[ChannelIndex] = ( SoundWave->ChannelSizes[ChannelIndex] / 2 );
							SampleCount = FMath::Max( SampleCount, SampleCounts[ChannelIndex] );
							SampleCounts[ChannelIndex] -= FirstSample;
						}
#endif
					}

					FirstSample = FMath::Min( SampleCount, FirstSample );
					LastSample = FMath::Min( SampleCount, LastSample );

					int16 *SamplePtr = reinterpret_cast<int16*>( WaveInfo.SampleDataStart );
					if( NumChannels <= 2 )
					{
						SamplePtr += FirstSample;
					}

					uint32 SamplesPerAmplitude = ( LastSample - FirstSample ) / AmplitudeBuckets;
					uint32 ExcessSamples = ( LastSample - FirstSample ) % AmplitudeBuckets;

					for( int32 AmplitudeIndex = 0; AmplitudeIndex < AmplitudeBuckets; ++AmplitudeIndex )
					{
						if( NumChannels <= 2 )
						{
							int64 SampleSum[2] = { 0 };
							uint32 SamplesToRead = SamplesPerAmplitude + ( ExcessSamples-- > 0 ? 1 : 0 );
							for( uint32 SampleIndex = 0; SampleIndex < SamplesToRead; ++SampleIndex )
							{
								for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
								{
									SampleSum[ChannelIndex] += FMath::Abs( *SamplePtr );
									SamplePtr++;
								}
							}
							for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
							{
								OutAmplitudes[( bSplitChannels ? ChannelIndex : 0 )][AmplitudeIndex] = SampleSum[ChannelIndex] / (float)SamplesToRead;
							}
						}
						else
						{
							UE_LOG( LogTemp, Log, TEXT( "WARNING, Amplitude : Not supported in packed" ) );
#if WITH_EDITORONLY_DATA
							uint32 SamplesRead = 0;
							int64 SampleSum = 0;
							uint32 SamplesToRead = SamplesPerAmplitude + ( ExcessSamples-- > 0 ? 1 : 0 );

							for( int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex )
							{
								uint32 SamplesToReadForChannel = FMath::Min( SamplesToRead, SampleCounts[ChannelIndex] );

								if( SamplesToReadForChannel > 0 )
								{
									if( bSplitChannels )
									{
										SampleSum = 0;
									}

									for( uint32 SampleIndex = 0; SampleIndex < SamplesToReadForChannel; ++SampleIndex )
									{
										SampleSum += FMath::Abs( *( SamplePtr + FirstSample + SampleIndex + SoundWave->ChannelOffsets[ChannelIndex] / 2 ) );
									}

									if( bSplitChannels )
									{
										OutAmplitudes[ChannelIndex][AmplitudeIndex] = SampleSum / (float)SamplesToReadForChannel;
									}
									SamplesRead += SamplesToReadForChannel;
									SampleCounts[ChannelIndex] -= SamplesToReadForChannel;
								}
							}

							if( !bSplitChannels )
							{
								OutAmplitudes[0][AmplitudeIndex] = SampleSum / (float)SamplesRead;
							}

							FirstSample += SamplesToRead;
#endif
						}
					}
				}
				else
				{
					UE_LOG( LogTemp, Warning, TEXT( "WaveHeader not found" ) );
				}
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "No BulkData found " ) );
			}
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "AmplitudeBuckets or NumChannels <= 0" ) );
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "No AudioData loaded" ) );
	}
}

