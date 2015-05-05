// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Timer.generated.h"

/**
 * 
 */
UCLASS( BlueprintType )
class SOUNDDUNGEON_MASTER_API UTimer : public UObject
{
	GENERATED_BODY()
	
public:
	UTimer();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	void Start();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	void Stop();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	void Reset();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	void Update( float DeltaTime );

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	float GetActiveTime();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	bool IsActive();

protected:

	float ActiveTime;

	bool isActive;
};
