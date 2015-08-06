

#include "SoundDungeon_Master.h"
#include "TubeBase.h"


// Sets default values
ATubeBase::ATubeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMesh" ) );
	RootComponent = MeshComponent;

	WavName = "MainVocal";
	Height = 100;
	Width = 300;
	Radius = 250;

	if( BucketList.Num() == 0 && GetWorld() )
	{
		UWAVLibrary* Instance = UWAVLibrary::GetInstance();

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
			UWAVLibrary::GetInstance()->GenerateTube( Data, &Config, this, BucketList );
		}
	}
}

void ATubeBase::BeginPlay()
{
	Super::BeginPlay();
	
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
