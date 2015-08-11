

#include "SoundDungeon_Master.h"
#include "TubeBase.h"


// Sets default values
ATubeBase::ATubeBase( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMesh" ) );
	RootComponent = MeshComponent;

	WavName = "MainVocal";
	Height = 100;
	Width = 30;
	Radius = 190;
	bEnablePlayGeneration = true;
}

void ATubeBase::BeginPlay()
{
	Super::BeginPlay();
	
	if( bEnablePlayGeneration )
	{
		InitTube();
	}
}

/*
void ATubeBase::BeginDestroy()
{
	if( BucketList.Num() != 0 )
	{
		for( auto& Element : BucketList )
		{
			Element->Destroy();
		}
		BucketList.Empty();
	}
}
*/

void ATubeBase::InitTube()
{
	UWAVLibrary* Instance = UWAVLibrary::GetInstance();

	if( GetWorld() )
	{
		if( !Instance->IsLoadedWAV( *( WavName ) ) )
		{
			Instance->LoadWAV( *( WavName ) );
		}

		TArray<uint8>* Data = Instance->GetWAV( *( WavName ) );

		if( Data )
		{
			FWaveformConfig Config;
			Config.Width = Width;
			Config.Height = Height;
			Config.Radius = Radius;
			Instance->GenerateTube( Data, &Config, this, BucketList );
		}
	}
}
