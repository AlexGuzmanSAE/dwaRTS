
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceCollectable.h"
#include "CommonEnum.h"
#include "Base_Resource.generated.h"


struct ResourcePair
{
	EResourceType rType;
	float amount;
};

UCLASS()
class STARTS_API ABase_Resource : public AActor, public IResourceCollectable
{
	GENERATED_BODY()
	
public:	
	ABase_Resource();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Type, meta = (AllowPrivateAccess = "true"));
	EResourceType resourceType;
	ResourcePair structTypePair;
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual ResourcePair ExtractResource_Implementation(float AmountToExtract);
};
