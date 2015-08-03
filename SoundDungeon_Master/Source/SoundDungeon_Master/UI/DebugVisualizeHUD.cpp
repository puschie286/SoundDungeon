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

	Storage.Empty( /* 100 */ );

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
		float Time = (float)FPlatformTime::Seconds();

		for( auto& Element : Storage )
		{
			LineItem.Origin = LastPoint;
			LineItem.EndPos = FVector( GraphWidth - ( ( Time - Element.Key ) / GraphLength ) * GraphWidth, Element.Value, 0.f );
			UE_LOG( LogTemp, Log, TEXT( "X Position : %f" ), GraphWidth - ( ( Time - Element.Key ) / GraphLength ) * GraphWidth );
			LastPoint = LineItem.EndPos;
			Canvas->DrawItem( LineItem );
		}
	}
}

void ADebugVisualizeHUD::AddValue( float NewValue )
{
	InputScale.AddValue( NewValue );
	Storage.Add( FPlatformTime::Seconds(), InputScale.GetScaled( NewValue ) );
	UE_LOG( LogTemp, Log, TEXT( "Add Value %f - ( Time : %f ) Elements : %d" ), InputScale.GetScaled( NewValue ), FPlatformTime::Seconds(), Storage.Num() );
}

void ADebugVisualizeHUD::UpdateStorage()
{
	float LastEntryTime = FPlatformTime::Seconds() - GraphLength;
	
	for( auto& Elements : Storage )
	{
		if( Elements.Key < LastEntryTime )
		{
			Storage.Remove( Elements.Key );
		}
	}

	Storage.KeySort(
		[]( float A, float B ) -> bool
		{
			return A > B;
		}
	);
}

void ADebugVisualizeHUD::Init()
{
	if( !bInit )
	{
		ScreenSize = GEngine->GameViewport->Viewport->GetSizeXY();

		bInit = true;
	}
}
