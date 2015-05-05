// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Timer.generated.h"

/**
 * 
 */
UCLASS()
class SOUNDDUNGEON_MASTER_API UTimer : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	void Start();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	void Stop();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	void Reset();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	void Update( float DeltaTime );

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	float GetTime();

	UFUNCTION( BlueprintCallable, Category = "Timer" )
	bool IsActive();

protected:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
	bool Active;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
	float Time;
};
