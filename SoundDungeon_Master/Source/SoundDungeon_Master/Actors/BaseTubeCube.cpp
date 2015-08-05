

#include "SoundDungeon_Master.h"
#include "BaseTubeCube.h"

ABaseTubeCube::ABaseTubeCube( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	static const ConstructorHelpers::FObjectFinder<UStaticMesh> BaseCube( TEXT("/Engine/BasicShapes/Cube") );
	GetStaticMeshComponent()->SetStaticMesh( BaseCube.Object );
	GetStaticMeshComponent()->SetMobility( EComponentMobility::Movable );
}
