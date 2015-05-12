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

// Blueprint functions
void UWAVLibrary::LIBLoadWAV( FString WAVName )
{
	UWAVLibrary::GetInstance()->LoadWAV( *( WAVName ) );
}

void UWAVLibrary::LIBUnloadWAV( FString WAVName )
{
	UWAVLibrary::GetInstance()->UnloadWAV( *( WAVName ) );
}

void UWAVLibrary::LIBFinishedUsage( FString WAVName )
{
	UWAVLibrary::GetInstance()->FinishedUsage( *( WAVName ) );
}

void UWAVLibrary::LIBGetWAV( FString WAVName, TArray<uint8>& OutData )
{
	OutData = *UWAVLibrary::GetInstance()->GetWAV( *( WAVName ) );
}