// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"

#include "Scaling.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = ( Custom ), Meta = ( BlueprintSpawnableComponent ) )
class SOUNDDUNGEON_MASTER_API UScaling : public UObject
{
	GENERATED_BODY()

private:
	float MinValue, MaxValue;
	float MinScale, MaxScale; // Value scaled between ScaleMin & ScaleMax

	float CurrentMax;
	float CurrentMin;

	bool bUseMinMaxValue;

public:
	UScaling();
	~UScaling();

	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void GetScaleMinMax( FVector2D& MinMaxScale ) const;
	float GetScaleMin() const;
	float GetScaleMax() const;
	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void SetScaleMin( const float Value );
	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void SetScaleMax( const float Value );

	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void GetValueMinMax( FVector2D& MinMaxValue ) const;
	float GetValueMin() const;
	float GetValueMax() const;
	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void SetValueMin( const float Value );
	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void SetValueMax( const float Value );

	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void GetCurrentMinMax( FVector2D& MinMaxCurrent ) const;
	float GetCurrentMin() const;
	float GetCurrentMax() const;
	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void SetCurrentMin( const float Value );
	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void SetCurrentMax( const float Value );

	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void UseMinMaxValue( bool& Value ) const;
	bool UseMinMaxValue() const;
	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void SetUseMinMaxValue( const bool ShouldUse );

	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void AddValue( const float Value );
	float GetScaled( const float Value ) const;

	UFUNCTION( BlueprintCallable, Category = "Scaling" )
	void GetScaled( const float Value, float& Scaled ) const;
};
