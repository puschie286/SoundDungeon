#pragma once

#include "Engine/StaticMeshActor.h"
#include "BaseTubeCube.generated.h"

/**
 * 
 */
UCLASS()
class SOUNDDUNGEON_MASTER_API ABaseTubeCube : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly )
	class ATubeBase* ParentRef;

	ABaseTubeCube( const FObjectInitializer& ObjectInitializer );

	virtual void BeginPlay() override;
};
