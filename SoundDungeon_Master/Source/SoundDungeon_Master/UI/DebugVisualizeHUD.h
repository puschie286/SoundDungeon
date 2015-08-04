#pragma once

#include "GameFramework/HUD.h"

#include <forward_list>

#include "../Helper/WAVLibrary.h"

#include "DebugVisualizeHUD.generated.h"

struct Data
{
	double Time;
	float Value;

	Data( double fTime, float fValue )
	{
		Time = fTime;
		Value = fValue;
	}
	Data()
	{
		Time = 0.f;
		Value = 0.f;
	}
};

/**
 * 
 */
UCLASS()
class SOUNDDUNGEON_MASTER_API ADebugVisualizeHUD : public AHUD
{
	GENERATED_BODY()

	std::forward_list<Data> Storage;

	FVector2D ScreenSize;

	int32 SizeWidth;

protected:
	/* Show/Hide Graph */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "DebugHUD" )
	bool bDrawGraph;

	/* Get Each Frame SharedData 0 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "DebugHUD" )
	bool bAutoUpdate;

	/* Length in Seconds */
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "DebugHUD" )
	float GraphLength;

	/* Height in % of Screen Height */
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "DebugHUD" )
	float GraphHeight;

	/* Width in % of Screen Width */
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "DebugHUD" )
	float GraphWidth;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "DebugHUD" )
	FScaling InputScale;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "DebugHUD" )
	int32 Slot;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "DebugHUD" )
	int32 Index;

	class UWAVLibrary* WavLibRef;

public:
	ADebugVisualizeHUD( const FObjectInitializer& ObjectInitializer );

	/* Draw Call for HUD */
	virtual void DrawHUD() override;

	/* Init Data */
	virtual void BeginPlay() override;

public:
	UFUNCTION( BlueprintCallable, Category = "DebugHUD" )
	void AddValue( float NewValue );

private:
	void UpdateStorage();
};
