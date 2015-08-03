#pragma once

#include "GameFramework/HUD.h"

#include "Containers/Map.h"

#include "DebugVisualizeHUD.generated.h"

/**
 * 
 */
UCLASS()
class SOUNDDUNGEON_MASTER_API ADebugVisualizeHUD : public AHUD
{
	GENERATED_BODY()

	TMap<float,float> Storage;

	FScaling InputScale;

	FVector2D ScreenSize;

	bool bInit;
protected:
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "DebugHUD" )
	bool bDrawGraph;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "DebugHUD" )
	bool bAutoUpdate;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "DebugHUD" )
	float GraphLength;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "DebugHUD" )
	int32 GraphHeight;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "DebugHUD" )
	int32 GraphWidth;

	class UWAVLibrary*					WavLibRef;

public:
	ADebugVisualizeHUD( const FObjectInitializer& ObjectInitializer );

	/* Draw Call for HUD */
	virtual void DrawHUD() override;

public:
	UFUNCTION( BlueprintCallable, Category = "DebugHUD" )
	void AddValue( float NewValue );

private:
	void UpdateStorage();
	void Init();
};
