// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "ExtendedAudioComponent.h"


UExtendedAudioComponent::UExtendedAudioComponent( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	PrimaryComponentTick.bCanEverTick = true;

	PlayTime = 0.f;
}

void UExtendedAudioComponent::PlaySound( float StartTime )
{
	if( Sound )
	{
		if( StartTime >= 0.f && StartTime < Sound->Duration )
		{
			this->PlayTime = StartTime;
			this->Play( StartTime );
		}
	}
}

void UExtendedAudioComponent::StopSound()
{
	if( Sound )
	{
		this->PlayTime = 0.f;
		this->Stop();
	}
}

void UExtendedAudioComponent::PauseSound()
{
	if( Sound )
	{
		this->Stop();
	}
}

void UExtendedAudioComponent::UnpauseSound()
{
	if( Sound )
	{
		if( this->PlayTime >= 0.f && this->PlayTime < Sound->Duration )
		{
			this->Play( this->PlayTime );
		}
	}
}

void UExtendedAudioComponent::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( this->IsPlaying() )
	{
		this->PlayTime += DeltaTime;
	}
}
