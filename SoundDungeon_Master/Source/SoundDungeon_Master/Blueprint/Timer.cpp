// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "Timer.h"

UTimer::UTimer()
{
	ActiveTime = 0.f;
	isActive = false;
}

void UTimer::Start()
{
	isActive = true;
}

void UTimer::Stop()
{
	isActive = false;
}

void UTimer::Reset()
{
	ActiveTime = 0.f;
}

void UTimer::Update( float DeltaTime )
{
	if( isActive )
	{
		ActiveTime += DeltaTime;
	}
}

float UTimer::GetActiveTime()
{
	return ActiveTime;
}

bool UTimer::IsActive()
{
	return isActive;
}
