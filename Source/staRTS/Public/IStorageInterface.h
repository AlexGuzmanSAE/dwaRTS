
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IStorageInterface.generated.h"

UINTERFACE(MinimalAPI)
class UIStorageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *	
 */
class STARTS_API IIStorageInterface
{
	GENERATED_BODY()

public:
	/**
	 * Deposit resources into storage
	 * @param ResourceType Type of resource being deposited
	 * @param Amount Amount to deposit
	 * @return Amount actually deposited (may be less if storage is full)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	float DepositResource(EResourceType ResourceType, float Amount);

	/**
	 * Check if storage can accept more of a specific resource
	 * @param ResourceType Type of resource to check
	 * @return True if storage has capacity
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	bool CanAcceptResource(EResourceType ResourceType) const;

	/**
	 * Get current amount of a specific resource in storage
	 * @param ResourceType Type of resource to query
	 * @return Current amount stored
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	float GetStoredAmount(EResourceType ResourceType) const;

	/**
	 * Get total capacity for a specific resource type
	 * @param ResourceType Type of resource to query
	 * @return Maximum capacity
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Storage")
	float GetMaxCapacity(EResourceType ResourceType) const;
};