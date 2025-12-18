#pragma once

#include "AttributeSet.h"
#include "FResourceMessage.generated.h"

USTRUCT(BlueprintType)
struct FResourceMessage
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Resource")	
	TArray<FGameplayAttribute> resourceAttributes;
};
