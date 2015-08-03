#include "SoundDungeon_Master.h"
#include "DebugVisualizeHUD.h"

ADebugVisualizeHUD::ADebugVisualizeHUD( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	bDrawGraph = false;
	bAutoUpdate = false;
	GraphLength = 5.f;

	GraphHeight = 500;
	GraphWidth = 1000;

	InputScale.MinScale = 0.f;
	InputScale.MaxScale = GraphHeight - 10;

	WavLibRef = UWAVLibrary::GetInstance();
	bInit = false;
}

void ADebugVisualizeHUD::DrawHUD()
{
	Init();

	if( bAutoUpdate )
	{
		// Add different Modes
		float Data;
		WavLibRef->GetShareSingle( Data );
		AddValue( Data );
	}

	if( bDrawGraph )
	{
		UpdateStorage();

		FCanvasLineItem LineItem;
		FVector LastPoint = FVector( 0.f, 0.f, 0.f );
		
		//UE_LOG( LogTemp, Log, TEXT( "Start Draw" ) );
		for( auto& Element : Storage )
		{
			LineItem.Origin = LastPoint;
			LineItem.EndPos = FVector( GraphWidth - ( ( FPlatformTime::Seconds() - Element.Time ) / GraphLength ) * GraphWidth, Element.Value, 0.f );
			//UE_LOG( LogTemp, Log, TEXT( "X Position : %f, Distance : %d" ), LineItem.EndPos.X, Time );
			LastPoint = LineItem.EndPos;
			Canvas->DrawItem( LineItem );
		}
	}
}

void ADebugVisualizeHUD::AddValue( float NewValue )
{
	InputScale.AddValue( NewValue );
	Data Values( FPlatformTime::Seconds(), InputScale.GetScaled( NewValue ) );
	Storage.push_front( Values );
	//UE_LOG( LogTemp, Log, TEXT( "Add Value %f - ( Time : %d ) Elements : %d" ), InputScale.GetScaled( NewValue ), FPlatformTime::Seconds(), std::distance( Storage.begin(), Storage.end() ) );
}

void ADebugVisualizeHUD::UpdateStorage()
{
	double LastEntryTime = FPlatformTime::Seconds() - GraphLength;
	
	for( auto StorageIT = Storage.begin(); StorageIT != Storage.end(); ++StorageIT )
	{
		if( (*StorageIT).Time < LastEntryTime )
		{
			Storage.erase_after( StorageIT, Storage.end() );
			break;
		}
	}
}

void ADebugVisualizeHUD::Init()
{
	if( !bInit )
	{
		ScreenSize = GEngine->GameViewport->Viewport->GetSizeXY();

		bInit = true;
	}
}
