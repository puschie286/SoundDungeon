// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "WAVLibrary.h"

UWAVLibrary::UWAVLibrary()
{
	this->AddToRoot();
	bUseLog = true;
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
				DataStorage.Add( WAVName, TempWAVData );

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
		else if( bUseLog )
		{
			UE_LOG( LogTemp, Log, TEXT( "WAV File %s allready loaded." ), *( WAVName.ToString() ) );
		}
	}
	return false;
}

bool UWAVLibrary::UnloadWAV( FName WAVName )
{
	if( CheckFName( WAVName ) )
	{
		if( DataStorage.Contains( WAVName ) )
		{
			DataStorage.Remove( WAVName );
			return true;
		}
		else if( bUseLog )
		{
			UE_LOG( LogTemp, Log, TEXT( "WAV File %s not loaded" ), *( WAVName.ToString() ) );
		}
	}
	return false;
}

TArray<uint8>* UWAVLibrary::GetWAV( FName WAVName )
{
	if( CheckFName )
	{
		if(  )
	}
	return NULL;
}
