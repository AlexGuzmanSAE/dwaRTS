
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ResourceCollectable.generated.h"

struct ResourcePair;

UINTERFACE(MinimalAPI)
class UResourceCollectable : public UInterface
{
	GENERATED_BODY()
};


class STARTS_API IResourceCollectable
{
	GENERATED_BODY()

public:
	virtual ResourcePair ExtractResource(float AmountToExtract) = 0;
};
