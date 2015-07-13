

#pragma once

#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UENUM( BlueprintType )
enum class EInteractionRangeState : uint8
{
	NONE	UMETA( DisplayName = "None" ),
	LONG	UMETA( DisplayName = "Long" ),
	SHORT	UMETA( DisplayName = "Short" )
};

UCLASS()
class SOUNDDUNGEON_MASTER_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent( class UInputComponent* InputComponent ) override;
	
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Default )
	bool	bRangeCheckEnabled;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Default )
	uint32	CheckPeriod;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Default )
	float	CheckRange;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Default )
	float	InteractiveRange;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Default )
	FName	InteractTag;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Default )
	FVector	GrabOffset;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Default )
	FName	GrabTag;

	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, Category = Default )
	AActor* GrabedTarget;

	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, Category = Default )
	EInteractionRangeState RangeCheckState;

private:
	uint32 CheckCounter;

public:

	UFUNCTION( BlueprintCallable, Category = "Character" )
	void GrabObject();

	UFUNCTION( BlueprintCallable, Category = "Character" )
	void ReleaseObject();

	UFUNCTION( BlueprintCallable, Category = "Character" )
	void InteractObject();

	UFUNCTION( BlueprintImplementableEvent, Category = "Character" )
	virtual void OnRangeCheckChange( EInteractionRangeState NewState );

private:
	void RangeCheck();

	AActor* RangeRaycast( float Range );
};
