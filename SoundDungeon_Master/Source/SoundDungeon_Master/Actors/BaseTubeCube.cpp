

#include "SoundDungeon_Master.h"
#include "BaseTubeCube.h"

ABaseTubeCube::ABaseTubeCube( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	static const ConstructorHelpers::FObjectFinder<UStaticMesh> BaseCube( TEXT("/Engine/BasicShapes/Cube") );
	UStaticMeshComponent* MeshPtr = GetStaticMeshComponent();
	MeshPtr->SetStaticMesh( BaseCube.Object );
	MeshPtr->SetMobility( EComponentMobility::Movable );
	MeshPtr->CastShadow = 0;
	MeshPtr->SetCollisionProfileName( FName( TEXT( "Trigger" ) ) );
	MeshPtr->bGenerateOverlapEvents = true;

	RootComponent = MeshPtr;
}
