
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IStorageInterface.generated.h"

UINTERFACE(MinimalAPI)
class UIStorageInterface : public UInterface
{
	GENERATED_BODY()
};


class STARTS_API IIStorageInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	float DepositResource(EResourceType ResourceType, float Amount);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	bool CanAcceptResource(EResourceType ResourceType) const;


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	float GetStoredAmount(EResourceType ResourceType) const;


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	float GetMaxCapacity(EResourceType ResourceType) const;
};