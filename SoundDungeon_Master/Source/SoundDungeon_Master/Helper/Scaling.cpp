// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundDungeon_Master.h"
#include "Scaling.h"

UScaling::UScaling()
{
	MaxScale = 1.f;
	MinScale = 0.f;

	MaxValue = 100.f;
	MinValue = -100.f;

	CurrentMax = 0.f;
	CurrentMin = 0.f;

	bUseMinMaxValue = false;
}

UScaling::~UScaling()
{
}

void UScaling::GetScaleMinMax( FVector2D& MinMaxScale ) const
{
	MinMaxScale.X = MinScale;
	MinMaxScale.Y = MaxScale;
}

float UScaling::GetScaleMin() const
{
	return MinScale;
}

float UScaling::GetScaleMax() const
{
	return MaxScale;
}

void UScaling::SetScaleMin( const float Value )
{
	MinScale = Value;
}

void UScaling::SetScaleMax( const float Value )
{
	MaxScale = Value;
}


void UScaling::GetValueMinMax( FVector2D& MinMaxValue ) const
{
	MinMaxValue.X = MinValue;
	MinMaxValue.Y = MaxValue;
}

float UScaling::GetValueMin() const
{
	return MinValue;
}

float UScaling::GetValueMax() const
{
	return MaxValue;
}

void UScaling::SetValueMin( const float Value )
{
	MinValue = Value;
}

void UScaling::SetValueMax( const float Value )
{
	MaxValue = Value;
}


void UScaling::GetCurrentMinMax( FVector2D& MinMaxCurrent ) const
{
	MinMaxCurrent.X = CurrentMin;
	MinMaxCurrent.Y = CurrentMax;
}

float UScaling::GetCurrentMin() const
{
	return CurrentMin;
}

float UScaling::GetCurrentMax() const
{
	return CurrentMax;
}

void UScaling::SetCurrentMin( const float Value )
{
	CurrentMin = Value;
}

void UScaling::SetCurrentMax( const float Value )
{
	CurrentMax = Value;
}


void UScaling::UseMinMaxValue( bool& Value ) const
{
	Value = bUseMinMaxValue;
}

bool UScaling::UseMinMaxValue() const
{
	return bUseMinMaxValue;
}

void UScaling::SetUseMinMaxValue( const bool ShouldUse )
{
	bUseMinMaxValue = ShouldUse;
}


void UScaling::AddValue( const float Value )
{
	auto SetValue = [&]( const float Value ) -> void
	{
		if( Value > CurrentMax )
		{
			CurrentMax = Value;
		}
		else if( Value < CurrentMin )
		{
			CurrentMin = Value;
		}
	};

	if( bUseMinMaxValue )
	{
		if( Value >= MinValue && Value <= MaxValue )
		{
			SetValue( Value );
		}
	}
	else
	{
		SetValue( Value );
	}
}

float UScaling::GetScaled( const float Value ) const
{
	float x = 0 - CurrentMin;
	float y = 0 - MinScale;
	return ( ( Value + x ) / ( CurrentMax + x ) ) * ( ( MaxScale + y ) - y );
}

void UScaling::GetScaled( const float Value, float& Scaled ) const
{
	Scaled = GetScaled( Value );
}
