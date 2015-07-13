

#include "SoundDungeon_Master.h"
#include "PlayerCharacter.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Defaults
	bRangeCheckEnabled = true;
	CheckPeriod = 5;
	CheckRange = 750.f;
	InteractiveRange = 500.f;
	InteractTag = FName( TEXT( "InteractiveAble" ) );

	GrabOffset = FVector( 200.f, 200.f, 200.f );
	GrabTag = FName( TEXT( "GrabAble" ) );
	GrabedTarget = nullptr;

	RangeCheckState = EInteractionRangeState::NONE;
	CheckCounter = 0;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( ++CheckCounter > CheckPeriod )
	{
		RangeCheck();
		CheckCounter = 0;
	}
}


void APlayerCharacter::GrabObject()
{

}

void APlayerCharacter::ReleaseObject()
{

}

void APlayerCharacter::InteractObject()
{

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void APlayerCharacter::RangeCheck()
{

}

AActor* APlayerCharacter::RangeRaycast( float Range )
{

}
