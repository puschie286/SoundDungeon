// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"

#include "Containers/Map.h"

#include "WAVLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOUNDDUNGEON_MASTER_API UWAVLibrary : public UObject
{
	GENERATED_BODY()
	
private:
	UWAVLibrary();

	static UWAVLibrary* Instance;

	TMap<FName, TArray<uint8>*> DataStorage;

	FString Path;

	bool CheckFName( FName Name );

public:

	static UWAVLibrary* GetInstance();

	bool LoadWAV( FName WAVName );

	bool UnloadWAV( FName WAVName );

	TArray<uint8>* GetWAV( FName WAVName );

	bool bUseLog;
};
