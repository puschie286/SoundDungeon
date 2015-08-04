// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "WAVLibrary.h"

#include "tools/kiss_fftnd.h"

// Helper

void GenerateLine( TArray<FCustomMeshTriangle>& List, FVector StartPos, FVector EndPos, bool DrawFull = false )
{
	FCustomMeshTriangle Triangle0, Triangle1;

	Triangle0.Vertex0 = FVector( StartPos.X, StartPos.Y, StartPos.Z );
	Triangle0.Vertex1 = FVector( EndPos.X, StartPos.Y, EndPos.Z );
	Triangle0.Vertex2 = FVector( StartPos.X, EndPos.Y, StartPos.Z );

	Triangle1.Vertex0 = Triangle0.Vertex1;
	Triangle1.Vertex1 = FVector( EndPos.X, EndPos.Y, EndPos.Z );
	Triangle1.Vertex2 = Triangle0.Vertex2;

	List.Add( Triangle0 );
	List.Add( Triangle1 );

	if( DrawFull )
	{
		FCustomMeshTriangle Triangle2;

		Triangle2.Vertex0 = Triangle0.Vertex1;
		Triangle2.Vertex1 = Triangle0.Vertex0;
		Triangle2.Vertex2 = Triangle0.Vertex2;

		Triangle1.Vertex0 = Triangle0.Vertex2;
		Triangle1.Vertex2 = Triangle0.Vertex1;

		List.Add( Triangle2 );
		List.Add( Triangle1 );
	}
}

// Static

UWAVLibrary::UWAVLibrary()
{
	this->AddToRoot();
	bUseLog = true;
	bUnloadWhenNotUsed = false;
	Path = FPaths::GameDir() + "Content\\AudioFiles\\";
	ShareStorage.SetNum( 5 );
}

UWAVLibrary* UWAVLibrary::GetInstance()
{
	if( !Instance )
	{
		Instance = NewObject<UWAVLibrary>();
	}
	return Instance;
}

UWAVLibrary* UWAVLibrary::Instance;

// private

bool UWAVLibrary::CheckFName( FName Name )
{
	if( !Name.IsNone() && Name.IsValid() )
	{
		return true;
	}
	else if( bUseLog )
	{
		UE_LOG( LogTemp, Warning, TEXT( "Invalid WAVName %s" ), *( Name.ToString() ) );
	}
	return false;
}

bool UWAVLibrary::CheckStorageContain( FName Name )
{
	if( DataStorage.Contains( Name ) )
	{
		return true;
	}
	else if( bUseLog )
	{
		UE_LOG( LogTemp, Log, TEXT( "WAV File %s not loaded" ), *( Name.ToString() ) );
	}
	return false;
}

// public

bool UWAVLibrary::LoadWAV( FName WAVName )
{
	if( CheckFName( WAVName ) )
	{
		if( !DataStorage.Contains( WAVName ) )
		{
			TArray<uint8>* TempWAVData = new TArray<uint8>();
			FString FullPath = Path + WAVName.ToString();

			if( !FullPath.Contains( ".wav" ) )
			{
				FullPath += ".wav";
			}

			bool loaded = FFileHelper::LoadFileToArray( *TempWAVData, FullPath.GetCharArray().GetData() );

			if( loaded )
			{
				TPair<uint8, TArray<uint8>*> TempDataPair;
				TempDataPair.Key = 0;
				TempDataPair.Value = TempWAVData;
				DataStorage.Add( WAVName, TempDataPair );

				if( bUseLog )
				{
					UE_LOG( LogTemp, Log, TEXT( "File %s load succesfull" ), *( WAVName.ToString() ) );
				}
				return true;
			}
			else if( bUseLog )
			{
				UE_LOG( LogTemp, Warning, TEXT( "Cant load File %s at Path : %s" ), *( WAVName.ToString() ), *( Path ) );
			}
		}
		else
		{
			if( bUseLog )
			{
				UE_LOG( LogTemp, Log, TEXT( "WAV File %s allready loaded." ), *( WAVName.ToString() ) );
			}
			return true;
		}
	}
	return false;
}

bool UWAVLibrary::UnloadWAV( FName WAVName )
{
	if( CheckFName( WAVName ) )
	{
		if( CheckStorageContain( WAVName ) )
		{
			DataStorage.Remove( WAVName );
			return true;
		}
	}
	return false;
}

bool UWAVLibrary::IsLoadedWAV( FName WAVName )
{
	if( CheckFName( WAVName ) )
	{
		if( CheckStorageContain( WAVName ) )
		{
			return true;
		}
	}
	return false;
}

TArray<uint8>* UWAVLibrary::GetWAV( FName WAVName )
{
	if( CheckFName( WAVName ) )
	{
		if( CheckStorageContain( WAVName ) )
		{
			++DataStorage[WAVName].Key;
			return( DataStorage[WAVName].Value );
		}
	}
	return NULL;
}

bool UWAVLibrary::FinishedUsage( FName WAVName )
{
	if( CheckFName( WAVName ) )
	{
		if( CheckStorageContain( WAVName ) )
		{
			--DataStorage[WAVName].Key;
			if( bUnloadWhenNotUsed && DataStorage[WAVName].Key == 0 )
			{
				if( bUseLog )
				{
					UE_LOG( LogTemp, Log, TEXT( "WAV unload : %s" ), *( WAVName.ToString() ) );
				}
				return UnloadWAV( WAVName );
			}
			return true;
		}
	}
	return false;
}

bool UWAVLibrary::FinishedUsage( TArray<uint8>* WavPtr )
{
	if( WavPtr != NULL )
	{
		for( auto pair : DataStorage )
		{
			if( pair.Value.Value == WavPtr )
			{
				--pair.Value.Key;
				if( bUnloadWhenNotUsed && pair.Value.Key == 0 )
				{
					if( bUseLog )
					{
						UE_LOG( LogTemp, Log, TEXT( "WAV unload : %s" ), *( pair.Key.ToString() ) );
					}
					return UnloadWAV( pair.Key );
				}
				return true;
			}
		}
		if( bUseLog )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Wav Ptr not found" ) );
		}
	}
	else if( bUseLog )
	{
		UE_LOG( LogTemp, Warning, TEXT( "NULL Pointer" ) );
	}
	return false;
}

bool UWAVLibrary::GenerateWaveform( TArray<uint8>* WavPtr, FWaveformConfig* WConfig )
{
	if( WavPtr != NULL )
	{
		// Copy from ThumbnailRendering
		uint8* RawWaveData = WavPtr->GetData();
		int32 RawDataSize = WavPtr->Num();
		FWaveModInfo WaveInfo;

		// parse the wave data
		if( WaveInfo.ReadWaveHeader( RawWaveData, RawDataSize, 0 ) )
		{
			uint8 numChannels = 1; // SoundWave->NumChannels
			const float SampleYScale = WConfig->Height / ( 2.f * 32767 * ( WConfig->DrawChannels ? numChannels : 1 ) );

			int16* SamplePtr = reinterpret_cast<int16*>( WaveInfo.SampleDataStart );

			uint32 SampleCount = 0;
			uint32 SampleCounts[10] = { 0 };

			if( numChannels <= 2 )
			{
				SampleCount = WaveInfo.SampleDataSize / ( 2 * numChannels );
			}
			else if( bUseLog )
			{
				UE_LOG( LogTemp, Warning, TEXT( "Support only for 2 Channels" ) );
			}
			const uint32 SamplesPerX = ( SampleCount / WConfig->Width ) + 1;
			float LastScaledSample[10] = { 0.f };

			TArray<FCustomMeshTriangle> TriangleList;
			const float RoundStep = 2 * PI / WConfig->Width;
			float LastRCos = ( WConfig->Radius != 0 ) ? ( WConfig->Radius ) : ( 0.f );
			float LastRSin = 0.f;
			for( int32 XOffset = 0; XOffset < WConfig->Width - 1; ++XOffset )
			{
				const float RCos = ( WConfig->Radius != 0 ) ? ( FMath::Cos( RoundStep * ( XOffset + 1 ) ) * WConfig->Radius ) : ( XOffset + 1 );
				const float RSin = ( WConfig->Radius != 0 ) ? ( FMath::Sin( RoundStep * ( XOffset + 1 ) ) * WConfig->Radius ) : ( 0.f );
				int64 SampleSum[10] = { 0 };
				if( numChannels <= 2 )
				{
					for( uint32 PerXSampleIndex = 0; PerXSampleIndex < SamplesPerX; ++PerXSampleIndex )
					{
						for( int32 ChannelIndex = 0; ChannelIndex < numChannels; ++ChannelIndex )
						{
							const int16 SampleValue = ( WConfig->DrawAsCurve ? *SamplePtr : FMath::Abs( *SamplePtr ) );
							SampleSum[ChannelIndex] += SampleValue;
							++SamplePtr;
						}
					}
				}
				if( WConfig->DrawChannels )
				{
					for( int32 ChannelIndex = 0; ChannelIndex < numChannels; ++ChannelIndex )
					{
						const float ScaledSample = SampleSum[ChannelIndex] / SamplesPerX * SampleYScale;
						if( WConfig->DrawAsCurve )
						{
							if( XOffset > 0 )
							{
								const float YCenter = ( ( 2 * ChannelIndex ) + 1 ) * WConfig->Height / ( 2.f * numChannels );
								GenerateLine( TriangleList, FVector( LastRCos - 1, YCenter + LastScaledSample[ChannelIndex], LastRSin ), FVector( RCos, YCenter + ScaledSample, RSin ) );
							}
							LastScaledSample[ChannelIndex] = ScaledSample;
						}
						else if( ScaledSample > 0.001f )
						{
							const float YCenter = ( ( 2 * ChannelIndex ) + 1 ) * WConfig->Height / ( 2.f * numChannels );
							GenerateLine( TriangleList, FVector( LastRCos, YCenter - ScaledSample, LastRSin ), FVector( RCos, YCenter + ScaledSample, RSin ) );
						}
					}
				}
				else
				{
					if( WConfig->DrawAsCurve )
					{
						float ScaledSampleSum = 0.f;
						int32 ActiveChannelCount = 0;
						for( int32 ChannelIndex = 0; ChannelIndex < numChannels; ++ChannelIndex )
						{
							const float ScaledSample = SampleSum[ChannelIndex] / SamplesPerX * SampleYScale;
							if( FMath::Abs( ScaledSample ) > 0.001f )
							{
								ScaledSampleSum += ScaledSample;
								++ActiveChannelCount;
							}
						}
						const float ScaledSample = ( ActiveChannelCount > 0 ? ScaledSampleSum / ActiveChannelCount : 0.f );
						if( XOffset > 0 )
						{
							GenerateLine( TriangleList, FVector( LastRCos - 1, 0.5f * WConfig->Height + LastScaledSample[0], LastRSin ), FVector( RCos, 0.5f * WConfig->Height + ScaledSample, RSin ) );
						}
						LastScaledSample[0] = ScaledSample;
					}
					else
					{
						float MaxScaledSample = 0.f;
						for( int32 ChannelIndex = 0; ChannelIndex < numChannels; ++ChannelIndex )
						{
							const float ScaledSample = SampleSum[ChannelIndex] / SamplesPerX * SampleYScale;
							MaxScaledSample = FMath::Max( MaxScaledSample, ScaledSample );
						}
						if( MaxScaledSample > 0.001f )
						{
							const float YCentre = 0.5f * WConfig->Height;
							GenerateLine( TriangleList, FVector( LastRCos, YCentre - MaxScaledSample, LastRSin ), FVector( RCos, YCentre + MaxScaledSample, RSin ), WConfig->DrawFull );
						}
					}
				}
				LastRCos = RCos;
				LastRSin = RSin;
			}
			WConfig->Component->SetCustomMeshTriangles( TriangleList );
			// Center alignment
			if( WConfig->Radius == 0 )
			{
				WConfig->Component->SetRelativeLocation( FVector( WConfig->Width / 2.f, 0.f, 0.f ) );
				WConfig->Component->SetRelativeRotation( FRotator( 180.f, 0.f, 90.f ) );
			}
			else
			{
				WConfig->Component->SetRelativeLocation( FVector( 0.f, 0.f, WConfig->Height / -2.f ) );
				WConfig->Component->SetRelativeRotation( FRotator( 180.f, 90.f, 90.f ) );
			}
		}
	}
	else if( bUseLog )
	{
		UE_LOG( LogTemp, Warning, TEXT( "NULL Pointer" ) );
	}
	return false;
}

void UWAVLibrary::CalculateFrequencySpectrum( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray<uint8>* InWavPtr, TArray<TArray<float>> &OutSpectrum )
{
	OutSpectrum.Empty();

	if( InWavPtr != NULL )
	{
		if( SpectrumWidth > 0 )
		{
			// Setup Output
			OutSpectrum.AddZeroed( ( bSplitChannels ? 2 : 1 ) );

			for( int32 ChannelIndex = 0; ChannelIndex < OutSpectrum.Num(); ++ChannelIndex )
			{
				OutSpectrum[ChannelIndex].AddZeroed( SpectrumWidth );
			}

			if( InWavPtr->Num() > 0 )
			{
				// Lock RAW Data
				uint8* RawWaveData = InWavPtr->GetData();
				int32 RawDataSize = InWavPtr->Num();
				FWaveModInfo WaveInfo;

				// Parse WaveData
				if( WaveInfo.ReadWaveHeader( RawWaveData, RawDataSize, 0 ) )
				{
					uint32 SampleCount = 0;
					uint32 SampleCounts[10] = { 0 };

					uint32 FirstSample = *WaveInfo.pSamplesPerSec * StartTime;
					uint32 LastSample = *WaveInfo.pSamplesPerSec * ( StartTime + TimeLength );

					SampleCount = WaveInfo.SampleDataSize / 4;

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

						{	
							for( int32 ChannelIndex = 0; ChannelIndex < 2; ++ChannelIndex )
							{
								buf[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC( sizeof( kiss_fft_cpx ) * SamplesToRead );
								out[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC( sizeof( kiss_fft_cpx ) * SamplesToRead );
							}
							
							SamplePtr += ( FirstSample * 2 );

							for( int32 SampleIndex = 0; SampleIndex < SamplesToRead; ++SampleIndex )
							{
								for( int32 ChannelIndex = 0; ChannelIndex < 2; ++ChannelIndex )
								{
									float FFTValue = *SamplePtr;
									FFTValue *= 0.5f * ( 1 - FMath::Cos( 2 * PI * SampleIndex / ( SampleCount - 1 ) ) );
									buf[ChannelIndex][SampleIndex].r = FFTValue;
									buf[ChannelIndex][SampleIndex].i = 0.f;

									SamplePtr++;
								}
							}
						}

						for( int32 ChannelIndex = 0; ChannelIndex < 2; ++ChannelIndex )
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
							int32 SamplesRead = 0;
							double SampleSum = 0;
							int32 SamplesForSpectrum = SamplesPerSpectrum + ( ExcessSamples-- > 0 ? 1 : 0 );

							for( int32 ChannelIndex = 0; ChannelIndex < 2; ++ChannelIndex )
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
						for( int32 ChannelIndex = 0; ChannelIndex < 2; ++ChannelIndex )
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
				UE_LOG( LogTemp, Warning, TEXT( "No Data found " ) );
			}
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "SpectrumWidth <= 0" ) );
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "WAV Data is NULL" ) );
	}
}

void UWAVLibrary::GetAmplitude( const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 AmplitudeBuckets, TArray<uint8>* InWavPtr, TArray<TArray<float>>& OutAmplitudes )
{
	OutAmplitudes.Empty();

	if( InWavPtr != NULL )
	{

		if( AmplitudeBuckets > 0 )
		{
			// Setup Output
			OutAmplitudes.AddZeroed( ( bSplitChannels ? 2 : 1 ) );

			for( int32 ChannelIndex = 0; ChannelIndex < OutAmplitudes.Num(); ++ChannelIndex )
			{
				OutAmplitudes[ChannelIndex].AddZeroed( AmplitudeBuckets );
			}

			if( InWavPtr->Num() > 0 )
			{
				uint8 *RawWaveData = InWavPtr->GetData();
				int32 RawDataSize = InWavPtr->Num();

				FWaveModInfo WaveInfo;

				// Parse WaveData
				if( WaveInfo.ReadWaveHeader( RawWaveData, RawDataSize, 0 ) )
				{
					uint32 SampleCount = 0;
					uint32 SampleCounts[10] = { 0 };

					uint32 FirstSample = *WaveInfo.pSamplesPerSec * StartTime;
					uint32 LastSample = *WaveInfo.pSamplesPerSec * ( StartTime + TimeLength );

					SampleCount = WaveInfo.SampleDataSize / ( 2 * 2 );

					FirstSample = FMath::Min( SampleCount, FirstSample );
					LastSample = FMath::Min( SampleCount, LastSample );

					int16 *SamplePtr = reinterpret_cast<int16*>( WaveInfo.SampleDataStart );

					SamplePtr += FirstSample;

					uint32 SamplesPerAmplitude = ( LastSample - FirstSample ) / AmplitudeBuckets;
					uint32 ExcessSamples = ( LastSample - FirstSample ) % AmplitudeBuckets;

					for( int32 AmplitudeIndex = 0; AmplitudeIndex < AmplitudeBuckets; ++AmplitudeIndex )
					{
						int64 SampleSum[2] = { 0 };
						uint32 SamplesToRead = SamplesPerAmplitude + ( ExcessSamples-- > 0 ? 1 : 0 );
						for( uint32 SampleIndex = 0; SampleIndex < SamplesToRead; ++SampleIndex )
						{
							for( int32 ChannelIndex = 0; ChannelIndex < 2; ++ChannelIndex )
							{
								SampleSum[ChannelIndex] += FMath::Abs( *SamplePtr );
								SamplePtr++;
							}
						}
						for( int32 ChannelIndex = 0; ChannelIndex < 2; ++ChannelIndex )
						{
							OutAmplitudes[( bSplitChannels ? ChannelIndex : 0 )][AmplitudeIndex] = SampleSum[ChannelIndex] / (float)SamplesToRead;
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
				UE_LOG( LogTemp, Warning, TEXT( "No Data found " ) );
			}
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "AmplitudeBuckets <= 0" ) );
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "WAV Data is NULL" ) );
	}
}

void UWAVLibrary::ClearShareData( const int32 NewSize /*= 0 */ )
{
	ShareStorage.Empty( NewSize );
}

void UWAVLibrary::GetShare( TArray<float>& OutData, const int32 Slot /*= 0 */ )
{
	if( ShareStorage.Num() > Slot )
	{
		OutData = ShareStorage[Slot];
	}
}

void UWAVLibrary::SetShare( const TArray<float>& InData, const int32 Slot /*= 0 */ )
{
	if( ShareStorage.Num() > Slot )
	{
		ShareStorage[Slot] = InData;
	}
	else
	{
		ShareStorage.Insert( InData, Slot );
	}
}

void UWAVLibrary::GetShareSingle( float& Out, const int32 Slot, const int32 Index )
{
	Out = -1;
	if( ShareStorage.Num() > Slot )
	{
		if( ShareStorage[Slot].Num() > Index )
		{
			Out = ShareStorage[Slot][Index];
		}
	}
}

void UWAVLibrary::CalculateWAVData( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, FString WAVName, int32 Slot )
{
	bool WAVLoaded = true;
	if( !IsLoadedWAV( *( WAVName ) ) )
	{
		WAVLoaded = LoadWAV( *( WAVName ) );
	}
	if( WAVLoaded )
	{
		TArray<float> *SlotData = &ShareStorage[Slot];
		TArray<float> Data0, Data1;
		LIBCalculateFrequencySpectrum( Channel, StartTime, TimeLength, SpectrumWidth, WAVName, Data0 );
		LIBGetAmplitude( Channel, StartTime, TimeLength, SpectrumWidth, WAVName, Data1 );

		if( SlotData->Num() != SpectrumWidth )
		{
			SlotData->Reserve( SpectrumWidth );
		}
		for( int32 i = 0; i < SpectrumWidth; ++i )
		{
			Data0[i] *= Data1[i];
		}
		*SlotData = Data0;
	}
}

// Blueprint functions
void UWAVLibrary::LIBLoadWAV( FString WAVName )
{
	GetInstance()->LoadWAV( *( WAVName ) );
}

void UWAVLibrary::LIBUnloadWAV( FString WAVName )
{
	GetInstance()->UnloadWAV( *( WAVName ) );
}

void UWAVLibrary::LIBFinishedUsage( FString WAVName )
{
	GetInstance()->FinishedUsage( *( WAVName ) );
}

void UWAVLibrary::LIBGetWAV( FString WAVName, TArray<uint8>& OutData, bool Force )
{
	if( GetInstance()->IsLoadedWAV( *( WAVName ) ) )
	{
		OutData = *GetInstance()->GetWAV( *( WAVName ) );
	}
	else if( Force )
	{
		GetInstance()->LoadWAV( *( WAVName ) );
		OutData = *GetInstance()->GetWAV( *( WAVName ) );
	}
}

void UWAVLibrary::LIBGenerateWaveform( TArray<uint8>& InData, FWaveformConfig& WConfig )
{
	GetInstance()->GenerateWaveform( &InData, &WConfig );
}

void UWAVLibrary::LIBCalculateFrequencySpectrum( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, FString WAVName, TArray<float> &OutSpectrum )
{
	OutSpectrum.Empty();
	bool WAVLoaded = true;
	if( !GetInstance()->IsLoadedWAV( *( WAVName ) ) )
	{
		WAVLoaded = GetInstance()->LoadWAV( *( WAVName ) );
	}
	if( WAVLoaded )
	{
		// Adjust Parameters
		if( SpectrumWidth <= 0 )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Adjust SpectrumWidth from %i to 1" ), SpectrumWidth );
			SpectrumWidth = 1;
		}
		if( Channel < 0 || Channel > 2 )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Adjust Channel from %i to 2" ), Channel );
			Channel = 2;
		}
		if( StartTime <= 0.f )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Adjust StartTime from %d to 0.01" ), StartTime );
			StartTime = 0.01f;
		}
		// Call
		TArray<TArray<float>> Spectrums;

		GetInstance()->CalculateFrequencySpectrum( ( Channel != 0 ), StartTime, TimeLength, SpectrumWidth, GetInstance()->GetWAV( *( WAVName ) ), Spectrums );
		// Output
		if( Spectrums.Num() != 0 )
		{
			if( Channel == 0 )
			{
				OutSpectrum = Spectrums[0];
			}
			else
			{
				OutSpectrum = Spectrums[Channel - 1];
			}
		}
	}
}

void UWAVLibrary::LIBGetAmplitude( int32 Channel, float StartTime, float TimeLength, int32 AmplitudeBuckets, FString WAVName, TArray<float> &OutAmplitudes )
{
	OutAmplitudes.Empty();
	bool WAVLoaded = true;
	if( !GetInstance()->IsLoadedWAV( *( WAVName ) ) )
	{
		WAVLoaded = GetInstance()->LoadWAV( *( WAVName ) );
	}
	if( WAVLoaded )
	{
		// Adjust Parameters
		if( StartTime <= 0.f )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Adjust StartTime from %d to 0.01s" ), StartTime );
			StartTime = 0.01f;
		}
		if( Channel < 0 || Channel > 2 )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Adjust Channel from %i to 2" ), Channel );
			Channel = 2;
		}
		// Call
		TArray<TArray<float>> Amplitudes;
		GetInstance()->GetAmplitude( ( Channel != 0 ), StartTime, TimeLength, AmplitudeBuckets, GetInstance()->GetWAV( *( WAVName ) ), Amplitudes );
		// Output
		if( Channel == 0 )
		{
			OutAmplitudes = Amplitudes[0];
		}
		else
		{
			OutAmplitudes = Amplitudes[Channel - 1];
		}
	}
}

void UWAVLibrary::LIBScaleAddValue( FScaling& Scale, const float Value )
{
	Scale.AddValue( Value );
}

void UWAVLibrary::LIBScaleGetScaled( FScaling& Scale, const float Value, float& ScaledValue )
{
	Scale.GetScaled( Value, ScaledValue );
}

void UWAVLibrary::LIBClearShareStorage( const int32 NewSize )
{
	GetInstance()->ClearShareData( NewSize );
}

void UWAVLibrary::LIBSetShare( TArray<float>& DataToStore, const int32 Slot /*= 0 */ )
{
	GetInstance()->SetShare( DataToStore, Slot );
}

void UWAVLibrary::LIBGetShare( TArray<float>& StoredData, const int32 Slot /*= 0 */ )
{
	GetInstance()->GetShare( StoredData, Slot );
}

void UWAVLibrary::LIBGetShareSingle( float& Out, const int32 Slot, const int32 Index )
{
	GetInstance()->GetShareSingle( Out, Slot, Index );
}

void UWAVLibrary::LIBArrayMultiply( const TArray<float>& FirstArray, const TArray<float>& SecondArray, TArray<float>& ResultArray )
{
	int32 ArraySize = FirstArray.Num();

	if( ArraySize == SecondArray.Num() )
	{
		if( ResultArray.Num() != ArraySize )
		{
			ResultArray.SetNum( ArraySize );
		}

		for( int32 i = 0; i < ArraySize; ++i )
		{
			ResultArray[i] = FirstArray[i] * SecondArray[i];
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "Cant multiply Arrays - Different Size" ) );
	}
}

void UWAVLibrary::LIBNormalized( const float Value, float &NormalizedValue, const float MinScale, const float MaxScale, const float MinValue, const float MaxValue )
{
	NormalizedValue = ( ( Value + ( 0 - MinValue ) ) / ( MaxValue + ( 0 - MinValue ) ) ) * ( MaxScale + ( 0 - MinScale ) ) - ( 0 - MinScale );
}

void UWAVLibrary::LIBCalculateWAVData( int32 Channel, float StartTime, float TimeLength, int32 SpectrumWidth, FString WAVName, int32 Slot )
{
	GetInstance()->CalculateWAVData( Channel, StartTime, TimeLength, SpectrumWidth, WAVName, Slot );
}

void UWAVLibrary::LIBUpdateWaveformCurser( UStaticMeshComponent* Target, float Duration, float PlayTime, int32 Radius, FRotator RotOffset, FVector PosOffset )
{
	check( Target );

	if( PlayTime != 0.f && Duration != 0.f )
	{
		float Point = ( PlayTime / Duration ) * 2 * PI;

		Target->SetRelativeLocation( FVector( FMath::Cos( Point ) * Radius, 0.f, FMath::Sin( Point ) * Radius ) + PosOffset );
		Target->SetRelativeRotation( FRotator( 0.f, ( PlayTime / Duration ) * -360.f, 0.f ) + RotOffset );
	}
}

void UWAVLibrary::LIBUpdateWaveformSelf( UCustomMeshComponent* Target, float Duration, float PlayTime )
{
	check( Target );

	if( PlayTime != 0.f && Duration != 0.f )
	{
		Target->SetRelativeRotation( FRotator( 180.f, 90.f + ( PlayTime / Duration ) * 360.f, 90.f ) );
	}
}
