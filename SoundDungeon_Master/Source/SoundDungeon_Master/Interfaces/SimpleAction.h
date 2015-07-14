#pragma once

#include "SimpleAction.generated.h"

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
};