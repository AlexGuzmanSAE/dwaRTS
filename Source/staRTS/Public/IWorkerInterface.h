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
	virtual void GatherResource(AActor* ResourceActor) = 0;
	virtual void DeliverResource(AActor* StorageActor) = 0;
	virtual bool IsCarryingResources() const = 0;
};
