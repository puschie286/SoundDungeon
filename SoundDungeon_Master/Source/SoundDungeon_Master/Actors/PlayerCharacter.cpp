

#include "SoundDungeon_Master.h"
#include "PlayerCharacter.h"

#include "../Interfaces/SimpleAction.h"


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
	Target = nullptr;

	RangeCheckState = EInteractionRangeState::NONE;
	CheckCounter = 0;

	GetCapsuleComponent()->InitCapsuleSize( 42.f, 96.f );
	TurnRate = 45.f;
	LookRate = 45.f;

	// Setup Camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>( TEXT( "CameraComponent" ) );
	CameraComponent->AttachParent = GetCapsuleComponent();
	CameraComponent->RelativeLocation = FVector( 0.f, 0.f, 64.f );
	CameraComponent->bUsePawnControlRotation = true;
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

	if( GrabedTarget )
	{
		GrabedTarget->SetActorLocation( GetActorLocation() + GetActorRotation().Vector() * GrabOffset );
	}
}

void APlayerCharacter::GrabObject()
{
	if( Target )
	{
		GrabedTarget = Target;
		GrabedTarget->DisableComponentsSimulatePhysics();
	}
}

void APlayerCharacter::ReleaseObject()
{
	if( GrabedTarget )
	{
		Cast<UPrimitiveComponent>( GrabedTarget->GetRootComponent() )->SetSimulatePhysics( true );
		GrabedTarget = nullptr;
	}
}

void APlayerCharacter::ActionObject()
{
	if( Target )
	{
		ISimpleAction* ActionInstance = Cast<ISimpleAction>( Target );
		
		if( Target->GetClass()->ImplementsInterface( USimpleAction::StaticClass() ) )
		{
			bool Result;
			ISimpleAction::Execute_Action( Target, GrabedTarget, Result );
			if( Result )
			{
				UE_LOG( LogTemp, Log, TEXT( "Interaction Successfull" ) );
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "Interaction Failed" ) );
			}
		}
	}
}

void APlayerCharacter::InteractObject()
{
	RangeCheck();

	if( RangeCheckState == EInteractionRangeState::SHORT )
	{
		if( Target->ActorHasTag( InteractTag ) )
		{
			ActionObject();
		}
		else if( Target->ActorHasTag( GrabTag ) )
		{
			GrabObject();
		}
	}
	else if( GrabedTarget )
	{
		ReleaseObject();
	}
}

void APlayerCharacter::RangeCheck()
{
	AActor* RangeTarget = RangeRaycast( CheckRange );
	EInteractionRangeState NewState = EInteractionRangeState::NONE;

	if( RangeTarget )
	{
		if( RangeTarget->ActorHasTag( GrabTag ) || RangeTarget->ActorHasTag( InteractTag ) )
		{
			if( InteractiveRange >= ( this->GetActorLocation() - RangeTarget->GetActorLocation() ).Size() )
			{
				NewState = EInteractionRangeState::SHORT;
			}
			else
			{
				NewState = EInteractionRangeState::LONG;
			}
			Target = RangeTarget;
		}
	}

	if( RangeCheckState != NewState )
	{
		RangeCheckState = NewState;
		OnRangeCheckChange( NewState );
	}
}

AActor* APlayerCharacter::RangeRaycast( float Range )
{
	FHitResult HitResult;
	FVector TraceStart, TraceEnd;
	FRotator CameraRotation;
	FCollisionQueryParams TraceParams( FName( TEXT( "RangeCheckTrace" ) ), false, this );

	if( GrabedTarget )
	{
		TraceParams.AddIgnoredActor( GrabedTarget );
	}
	
	this->GetInstigatorController()->GetPlayerViewPoint( TraceStart, CameraRotation );
	TraceEnd = TraceStart + CameraRotation.Vector() * Range;

	if( GetWorld()->LineTraceSingle( HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldDynamic, TraceParams ) )
	{
		return HitResult.GetActor();
	}

	return nullptr;
}
