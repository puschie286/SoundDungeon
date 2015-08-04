#include "SoundDungeon_Master.h"
#include "DebugVisualizeHUD.h"

ADebugVisualizeHUD::ADebugVisualizeHUD( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	bDrawGraph = false;
	bAutoUpdate = false;
	GraphLength = 5.f;

	GraphHeight = 50.f;
	GraphWidth = 50.f;
	
	SizeWidth = 600;

	InputScale.MinScale = 0.f;
	InputScale.MaxScale = 300.f - 10;

	WavLibRef = UWAVLibrary::GetInstance();
}

void ADebugVisualizeHUD::BeginPlay() 
{
	Super::BeginPlay();

	ScreenSize = GEngine->GameViewport->Viewport->GetSizeXY();

	SizeWidth = ScreenSize.X * ( GraphWidth / 100 );

	InputScale.MinScale = 0.f;
	InputScale.MaxScale = ScreenSize.Y * ( GraphHeight / 100 );

	WavLibRef = UWAVLibrary::GetInstance();
}

void ADebugVisualizeHUD::DrawHUD()
{
	if( bAutoUpdate )
	{
		float Data;
		WavLibRef->GetShareSingle( Data, Slot, Index );
		AddValue( Data );
	}

	if( bDrawGraph )
	{
		UpdateStorage();

		FCanvasLineItem LineItem;
		FVector LastPoint = FVector( SizeWidth, 0.f, 0.f );
		
		for( auto& Element : Storage )
		{
			LineItem.Origin = LastPoint;
			LineItem.EndPos = FVector( SizeWidth - ( ( FPlatformTime::Seconds() - Element.Time ) / GraphLength ) * SizeWidth, Element.Value, 0.f );
			LastPoint = LineItem.EndPos;
			Canvas->DrawItem( LineItem );
		}
	}
}

void ADebugVisualizeHUD::AddValue( float NewValue )
{
	Data Values( FPlatformTime::Seconds(), InputScale.GetScaled( NewValue ) );
	Storage.push_front( Values );
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
