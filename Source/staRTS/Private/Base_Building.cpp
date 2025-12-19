// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Building.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InputActionValue.h"
#include "IStorageInterface.h"
#include "EnhancedInputComponent.h"

ABase_Building::ABase_Building()
{ 	
	PrimaryActorTick.bCanEverTick = true;

	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh_Component"));
	RootComponent = meshComponent;

	selectedIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Selected_Indicator"));
	selectedIndicator->SetupAttachment(RootComponent);
	selectedIndicator->SetHiddenInGame(true);
	selectedIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABase_Building::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABase_Building::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABase_Building::SelectActor_Implementation(const bool isSelected)
{
	selectedIndicator->SetHiddenInGame(!isSelected);
}

void ABase_Building::EnablePlacingMode()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();

	if (playerController)
	{
		EnableInput(playerController);
		UEnhancedInputComponent* inputComponent = Cast<UEnhancedInputComponent>(playerController->InputComponent);
		if (inputComponent)
		{
			inputComponent->BindAction(placeBuildingAction, ETriggerEvent::Completed, this, &ABase_Building::PlaceBuilding);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(validationTimerHandle, this, &ABase_Building::CheckLocationValid, 0.03f, true);
	this->SetActorEnableCollision(false);
}

void ABase_Building::CheckLocationValid()
{
	ToggleBuildingValidity(canPlaceBuilding);

	canPlaceBuilding = false;

	FHitResult hitResult;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, hitResult);
	if (!hitResult.bBlockingHit)
	{
		return;
	}	

	SetActorLocation(hitResult.Location);

	if (!hitResult.GetActor()->ActorHasTag(requiredTag))
	{
		return;
	}

	FVector boxHelfExtent = buildAreaExtents / 2.f;
	FVector traceStart = hitResult.Location + FVector(0.f, 0.f, boxHelfExtent.Z);
	FVector traceEnd = hitResult.Location + FVector::UpVector;

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	queryParams.bTraceComplex = true;
	TArray<FHitResult> outHits;

	bool bHit = GetWorld()->SweepMultiByChannel(outHits, traceStart, traceEnd, GetActorRotation().Quaternion(), ECC_Visibility, FCollisionShape::MakeBox(boxHelfExtent), queryParams);

	for (const FHitResult& hit : outHits)
	{
		if (!(hit.GetActor() != nullptr && hit.GetActor()->ActorHasTag(requiredTag)))
		{
			return;
		}
	}
	canPlaceBuilding = true;
}

void ABase_Building::PlaceBuilding(const FInputActionValue& value)
{
	if (!canPlaceBuilding)
	{
		SetLifeSpan(0.1f);
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(validationTimerHandle);
	this->SetActorEnableCollision(true);
}

void ABase_Building::CancelBuildingPlacement(const FInputActionValue& value)
{

}

