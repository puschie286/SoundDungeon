// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "Timer.h"

void UTimer::Start()
{
	Active = true;
}

void UTimer::Stop()
{
	Active = false;
}

void UTimer::Reset()
{
	Active = false;
	Time = 0.f;
}

void UTimer::Update( float DeltaTime )
{
	if( Active )
	{
		Time += DeltaTime;
	}
}

float UTimer::GetTime()
{
	return Time;
}

bool UTimer::IsActive()
{
	return Active;
}