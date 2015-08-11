

#include "SoundDungeon_Master.h"
#include "TubePlayer.h"

#define PI2 PI * 2


// Sets default values
ATubePlayer::ATubePlayer( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Init
	Radius = 190;
	RightSpeed = 3.f;
	ForwardSpeed = 25.f;

	LastLocation = FVector();
	DefaultRotation = FRotator();
	RotatingState = 0.f;
	RightStep = 0.f;
	MoveRightState = 0;

	// Components
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>( TEXT( "CapsuleComponent" ) );
	CapsuleComponent->InitCapsuleSize( 42.f, 42.f );
	CapsuleComponent->bGenerateOverlapEvents = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>( TEXT( "CameraComponent" ) );
	CameraComponent->AttachParent = CapsuleComponent;
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetRelativeRotation( FRotator( 0.f, 0.f, 180.f ) );

	RootComponent = CapsuleComponent;
}

// Called when the game starts or when spawned
void ATubePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	LastLocation = GetActorLocation();
	DefaultRotation = GetActorRotation();
	RightStep = PI2 / RightSpeed;
}

// Called every frame
void ATubePlayer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( MoveRightState != 0 )
	{
		RotatingState += MoveRightState * DeltaTime * RightStep;
	}

	if( RotatingState > PI2 )
	{
		RotatingState -= PI2;
	}
	else if( RotatingState < 0 )
	{
		RotatingState += PI2;
	}

	SetActorLocation( 
		FVector(
			LastLocation.X + ForwardSpeed * -DeltaTime,
			LastLocation.Y + FMath::Sin( RotatingState ) * Radius,
			LastLocation.Z + FMath::Cos( RotatingState ) * Radius
		)
	);
	SetActorRotation( 
		FRotator( DefaultRotation.Pitch, DefaultRotation.Yaw, DefaultRotation.Roll + ( RotatingState / PI ) * -180.f )
	);

	LastLocation = FVector( LastLocation.X + ForwardSpeed * -DeltaTime, LastLocation.Y, LastLocation.Z );
}

// Called to bind functionality to input
void ATubePlayer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis( "MoveRight", this, &ATubePlayer::MoveRight );
}

void ATubePlayer::MoveRight( float Value )
{
	MoveRightState = (int8)Value;
}
