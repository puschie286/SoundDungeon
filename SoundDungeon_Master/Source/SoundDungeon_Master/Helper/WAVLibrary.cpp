// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "WAVLibrary.h"

UWAVLibrary::UWAVLibrary()
{
	this->AddToRoot();
	bUseLog = true;
	bUnloadWhenNotUsed = true;
	Path = FPaths::GameDir() + "Content\\AudioFiles\\";
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

bool UWAVLibrary::GenerateWaveform( FName WAVName, UCustomMeshComponent* InComponent )
{
	return GenerateWaveform( GetWAV( WAVName ), InComponent );
}

bool UWAVLibrary::GenerateWaveform( TArray<uint8>* WavPtr, UCustomMeshComponent* InComponent )
{
	if( WavPtr != NULL )
	{
		// coded parameter
		int32 X = 0;
		int32 Y = 0;
		uint32 Width = 300;
		uint32 Height = 100; 

		// TODO : Canvas
		FCanvas* Canvas = NULL;
		
		// Copy from ThumbnailRendering
		bool bDrawChannels = true;
		bool bDrawAsCurve = false;

		FCanvasLineItem LineItem;
		LineItem.SetColor( FLinearColor::White );
		
		uint8* RawWaveData = WavPtr->GetData();
		int32 RawDataSize = WavPtr->Num();
		FWaveModInfo WaveInfo;

		// parse the wave data
		if( WaveInfo.ReadWaveHeader( RawWaveData, RawDataSize, 0 ) )
		{
			uint8 numChannels = 2; // SoundWave->NumChannels
			const float SampleYScale = Height / ( 2.f * 32767 * ( bDrawChannels ? numChannels : 1 ) );

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
			const uint32 SamplesPerX = ( SampleCount / Width ) + 1;
			float LastScaledSample[10] = { 0.f };

			for( uint32 XOffset = 0; XOffset < Width - 1; ++XOffset )
			{
				int64 SampleSum[10] = { 0 };
				if( numChannels <= 2 )
				{
					for( uint32 PerXSampleIndex = 0; PerXSampleIndex < SamplesPerX; ++PerXSampleIndex )
					{
						for( int32 ChannelIndex = 0; ChannelIndex < numChannels; ++ChannelIndex )
						{
							const int16 SampleValue = ( bDrawAsCurve ? *SamplePtr : FMath::Abs( *SamplePtr ) );
							SampleSum[ChannelIndex] += SampleValue;
							++SamplePtr;
						}
					}
				}
				else if( bUseLog )
				{
					UE_LOG( LogTemp, Warning, TEXT( "Support only for 2 Channels" ) );
				}
				if( bDrawChannels )
				{
					for( int32 ChannelIndex = 0; ChannelIndex < numChannels; ++ChannelIndex )
					{
						const float ScaledSample = SampleSum[ChannelIndex] / SamplesPerX * SampleYScale;
						if( bDrawAsCurve )
						{
							if( XOffset > 0 )
							{
								const float YCenter = Y + ( ( 2 * ChannelIndex ) + 1 ) * Height / ( 2.f * numChannels );
								//LineItem.Draw( Canvas, FVector2D( X + XOffset - 1, YCenter + LastScaledSample[ChannelIndex] ), FVector2D( X + XOffset, YCenter + ScaledSample ) );
							}
							LastScaledSample[ChannelIndex] = ScaledSample;
						}
						else if( ScaledSample > 0.001f )
						{
							const float YCenter = Y + ( ( 2 * ChannelIndex ) + 1 ) * Height / ( 2.f * numChannels );
							//LineItem.Draw( Canvas, FVector2D( X + XOffset, YCenter - ScaledSample ), FVector2D( X + XOffset, YCenter + ScaledSample ) );
						}
					}
				}
				else
				{
					if( bDrawAsCurve )
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
							const float YCenter = Y + 0.5f * Height;
							//LineItem.Draw( Canvas, FVector2D( X + XOffset - 1, YCenter + LastScaledSample[0] ), FVector2D( X + XOffset, YCenter + ScaledSample ) );
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
							const float YCenter = Y + 0.5f * Height;
							//LineItem.Draw( Canvas, FVector2D( X + XOffset, YCenter - MaxScaledSample ), FVector2D( X + XOffset, YCenter + MaxScaledSample ) );
						}
					}
				}
			}
		}
	}
	else if( bUseLog )
	{
		UE_LOG( LogTemp, Warning, TEXT( "NULL Pointer" ) );
	}
	return false;
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

void UWAVLibrary::LIBGetWAV( FString WAVName, TArray<uint8>& OutData )
{
	OutData = *GetInstance()->GetWAV( *( WAVName ) );
}

void UWAVLibrary::LIBGetWaveformFromData( TArray<uint8>& InData, UCustomMeshComponent* InComponent )
{
	GetInstance()->GenerateWaveform( &InData, InComponent );
}

void UWAVLibrary::LIBGetWaveform( FString WAVName, UCustomMeshComponent* InComponent )
{
	GetInstance()->GenerateWaveform( *( WAVName ), InComponent );
}
