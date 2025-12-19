#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CommonEnum.h"
#include "ResourceCollectable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UResourceCollectable : public UInterface
{
	GENERATED_BODY()
};

class STARTS_API IResourceCollectable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource")
	FResourcePair ExtractResource(float AmountToExtract);
};