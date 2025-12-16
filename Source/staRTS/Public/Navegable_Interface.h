#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Navegable_Interface.generated.h"

UINTERFACE(MinimalAPI)
class UNavegable_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARTS_API INavegable_Interface
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Navegable_Interface")
	void MoveToLocation(const FVector targetLocation);

};
