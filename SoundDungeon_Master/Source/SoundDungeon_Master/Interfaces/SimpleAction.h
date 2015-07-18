#pragma once

#include "Interface.h"

#include "SimpleAction.generated.h"

UENUM( BlueprintType )
enum class EInteractionRangeState : uint8
{
	NONE	UMETA( DisplayName = "None" ),
	LONG	UMETA( DisplayName = "Long" ),
	SHORT	UMETA( DisplayName = "Short" )
};

UINTERFACE( MinimalAPI )
class USimpleAction : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ISimpleAction
{
	GENERATED_IINTERFACE_BODY()

	UFUNCTION( BlueprintImplementableEvent, Category = Action )
	void Action( AActor* Target, bool& Result );

	virtual bool Action( AActor* Target ) = 0;
};