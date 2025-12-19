#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceCollectable.h"
#include "CommonEnum.h"
#include "Base_Resource.generated.h"



UCLASS()
class STARTS_API ABase_Resource : public AActor, public IResourceCollectable
{
	GENERATED_BODY()
    
public: 
	ABase_Resource();

	// Resource configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	EResourceType resourceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float maxAmount = 1000.0f;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	float currentAmount;

	UPROPERTY(BlueprintReadOnly, Category = "Resource")
	FResourcePair structTypePair;

	// Called when resource is fully depleted
	virtual void OnResourceDepleted();

public: 
	virtual void Tick(float DeltaTime) override;

	// IResourceCollectable Interface
	virtual FResourcePair ExtractResource_Implementation(float AmountToExtract) override;

	// Blueprint accessible functions
	UFUNCTION(BlueprintCallable, Category = "Resource")
	float GetRemainingAmount() const;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	float GetMaxAmount() const;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool IsDepleted() const;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	float GetResourcePercentage() const;
};