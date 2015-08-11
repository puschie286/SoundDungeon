

#pragma once

#include "GameFramework/Pawn.h"
#include "TubePlayer.generated.h"

UCLASS()
class SOUNDDUNGEON_MASTER_API ATubePlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATubePlayer( const FObjectInitializer& ObjectInitializer );

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void MoveRight( float Value );

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
	class UCameraComponent* CameraComponent;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
	class UCapsuleComponent* CapsuleComponent;
	
	FORCEINLINE class UCameraComponent* GetCameraComponent() const
	{
		return CameraComponent;
	}

	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const
	{
		return CapsuleComponent;
	}

protected:
	UPROPERTY( VisibleInstanceOnly, BlueprintReadWrite, Category = "Tube" )
	int32	Radius;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Tube", meta = (  ToolTip = "Zeit in Sekunden die für eine Umrundung benötigt wird." ) )
	float	RightSpeed;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Tube", meta = (  ToolTip = "Angabe in cm/sek" ) )
	float	ForwardSpeed;

private:
	FVector LastLocation;
	FRotator DefaultRotation;
	float	RotatingState;
	float	RightStep;
	int8	MoveRightState;
};
