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
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
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
	AActor* Target;

	UPROPERTY( VisibleInstanceOnly, BlueprintReadOnly, Category = Default )
	EInteractionRangeState RangeCheckState;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default )
	float TurnRate;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default )
	float LookRate;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
	class UCameraComponent* CameraComponent;

private:
	uint32 CheckCounter;

public:

	UFUNCTION( BlueprintCallable, Category = "Character" )
	void GrabObject();

	UFUNCTION( BlueprintCallable, Category = "Character" )
	void ReleaseObject();

	UFUNCTION( BlueprintCallable, Category = "Character" )
	void ActionObject();

	UFUNCTION( BlueprintCallable, Category = "Character" )
	void InteractObject();

	UFUNCTION( BlueprintCallable, Category = "Character" )
	void RangeCheck();

	UFUNCTION( BlueprintImplementableEvent, Category = "Character" )
	virtual void OnRangeCheckChange( EInteractionRangeState NewState );

	FORCEINLINE class UCameraComponent* GetCameraComponent() const
	{
		return CameraComponent;
	}

private:
	AActor* RangeRaycast( float Range );
};
