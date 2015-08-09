#pragma once

#include "GameFramework/Actor.h"
#include "TubeBase.generated.h"

UCLASS()
class SOUNDDUNGEON_MASTER_API ATubeBase : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<AActor*> BucketList;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString WavName;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int32 Height;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int32 Width;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int32 Radius;
	
	class UStaticMeshComponent* MeshComponent;
	
public:	
	ATubeBase();

	virtual void BeginPlay() override;	

	// TODO : correct delete of BucketList Actors
	//virtual void BeginDestroy() override;

	UFUNCTION( BlueprintCallable, Category = "TubeBase" )
	void InitTube();
};
