

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISelectable.generated.h"

UINTERFACE(MinimalAPI)
class UISelectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARTS_API IISelectable
{
	GENERATED_BODY()

	
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selectable Interface")

	void SelectActor(const bool isSelected);
};
