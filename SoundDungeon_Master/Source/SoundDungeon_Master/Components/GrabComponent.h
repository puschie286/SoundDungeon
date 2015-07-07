// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GrabComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOUNDDUNGEON_MASTER_API UGrabComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabComponent();

	// Called when the game starts
	virtual void InitializeComponent() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	virtual void OnComponentDestroyed() override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	bool UsePhysicGrab;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	float GrabRange;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	FVector GrabDistance;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
	FName GrabTag;

	UFUNCTION( BlueprintCallable, Category = "Grab" )
	void GrabObject();

	UFUNCTION( BlueprintCallable, Category = "Grab" )
	void ReleaseObject();

	UFUNCTION( BlueprintPure, Category = "Grab" )
	bool HasGrabedObject();

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay )
	class AActor* GrabedTarget;

protected:
	class UPhysicsHandleComponent* PhysicHandle;
};
