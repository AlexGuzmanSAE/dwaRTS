// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Building.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

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

// Called every frame
void ABase_Building::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABase_Building::SelectActor_Implementation(const bool isSelected)
{
	selectedIndicator->SetHiddenInGame(!isSelected);
}

