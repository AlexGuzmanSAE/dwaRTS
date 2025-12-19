// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWorkerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UIWorkerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARTS_API IIWorkerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Worker")
	void GatherResource(AActor* ResourceActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Worker")
	void DeliverResource(AActor* StorageActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Worker")
	bool IsCarryingResources() const;
};
