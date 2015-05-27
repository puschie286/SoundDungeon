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

	void GenerateLine( TArray<FCustomMeshTriangle>& List, FVector2D StartPos, FVector2D EndPos, float Scale = 1.f );

public:

	static UWAVLibrary* GetInstance();

	bool LoadWAV( FName WAVName );

	bool UnloadWAV( FName WAVName );

	TArray<uint8>* GetWAV( FName WAVName );

	bool FinishedUsage( FName WAVName );

	bool FinishedUsage( TArray<uint8>* WavPtr );

	bool GenerateWaveform( TArray<uint8>* WavPtr, UCustomMeshComponent* InComponent, bool DrawChannels = false, bool DrawAsCurve = false, uint32 Width = 300, uint32 Height = 100 );

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
	static void LIBGetWAV( FString WAVName, TArray<uint8>& OutData );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBGetWaveform( FString WAVName, UCustomMeshComponent* InComponent, bool DrawChannels = false, bool DrawAsCurve = false, int32 Width = 300, int32 Height = 100 );

	UFUNCTION( BlueprintCallable, Category = "SoundVisualize" )
	static void LIBGetWaveformFromData( TArray<uint8>& InData, UCustomMeshComponent* InComponent, bool DrawChannels = false, bool DrawAsCurve = false, int32 Width = 300, int32 Height = 100 );
};
