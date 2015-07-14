// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "GrabComponent.h"


// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	
	bWantsInitializeComponent = true;
	// ...
	GrabRange = 500.f;
	GrabTag = FName( TEXT( "GrabAble" ) );
	GrabDistance = FVector( 200.f, 200.f, 150.f );
	GrabedTarget = nullptr;

	PhysicHandle = CreateDefaultSubobject<UPhysicsHandleComponent>( TEXT( "GrabPhysicsHelper" ) );
}

// Called when the game starts
void UGrabComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// ...
}


// Called every frame
void UGrabComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
	if( GrabedTarget != nullptr )
	{
		FVector NewLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorRotation().Vector() * GrabDistance;
		if( UsePhysicGrab )
		{
			PhysicHandle->SetTargetLocation( NewLocation );
			PhysicHandle->SetTargetRotation( GetOwner()->GetActorRotation() );
		}
		else
		{
			GrabedTarget->SetActorLocation( NewLocation );
			GrabedTarget->SetActorRotation( GetOwner()->GetActorRotation() );
		}
	}
}

void UGrabComponent::OnComponentDestroyed()
{
	PhysicHandle->DestroyComponent();

	Super::OnComponentDestroyed();
}

void UGrabComponent::GrabObject()
{
	if( GrabedTarget == nullptr )
	{
		// Trace
		FHitResult HitResult;
		FVector TraceStart, TraceEnd, TraceDirection;
		FRotator CamRotation;
		FCollisionQueryParams TraceParams( FName( TEXT( "GrabTrace" ) ), false, GetOwner() );

		GetOwner()->GetInstigatorController()->GetPlayerViewPoint( TraceStart, CamRotation );
		TraceDirection = CamRotation.Vector();
		TraceEnd = TraceStart + TraceDirection * GrabRange;

		if( GetWorld()->LineTraceSingle( HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldDynamic, TraceParams ) )
		{
			UE_LOG( LogTemp, Log, TEXT( "GrabTrace reach Object %s" ), *( HitResult.GetActor()->GetName() ) );
			if( HitResult.GetActor()->ActorHasTag( GrabTag ) )
			{
				UE_LOG( LogTemp, Log, TEXT( "GrabTarget accepted" ) );
				GrabedTarget = HitResult.GetActor();

				if( UsePhysicGrab )
				{
					PhysicHandle->GrabComponent( HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, false );
				}
				else
				{
					GrabedTarget->DisableComponentsSimulatePhysics();
				}
			}
		}
	}
}

void UGrabComponent::ReleaseObject()
{
	if( GrabedTarget != nullptr )
	{
		if( UsePhysicGrab )
		{
			PhysicHandle->ReleaseComponent();
			Cast<UPrimitiveComponent>( GrabedTarget->GetRootComponent() )->WakeAllRigidBodies();
		}
		else
		{
			Cast<UPrimitiveComponent>( GrabedTarget->GetRootComponent() )->SetSimulatePhysics( true );
		}
		GrabedTarget = nullptr;
	}
}

bool UGrabComponent::HasGrabedObject()
{
	return ( GrabedTarget != nullptr );
}