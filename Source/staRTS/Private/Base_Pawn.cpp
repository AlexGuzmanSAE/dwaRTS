// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "NavigationSystem.h"

ABase_Pawn::ABase_Pawn()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule_Component"));
	capsule->InitCapsuleSize(32.f, 96.0f);
	RootComponent = capsule;
	capsule->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	capsule->SetCanEverAffectNavigation(false);

	capsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	capsule->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);


	skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal_Mesh"));
	skeletalMesh->SetupAttachment(RootComponent);

	floatPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating_Pawn_Movement"));

	selectedIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Selected_Indicator"));
	selectedIndicator->SetupAttachment(RootComponent);
	selectedIndicator->SetHiddenInGame(true);
	selectedIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	selectedIndicator->SetCanEverAffectNavigation(false);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void ABase_Pawn::BeginPlay()
{
	Super::BeginPlay();
}

void ABase_Pawn::Move()
{
	if(!bMoving)
	{
		return;
	}

	FVector moveDirection = moveTargetLocation - GetActorLocation();
	if (moveDirection.Length() < minDistanceToTarget)
	{
		bMoving = false;
		return;
	}

	moveDirection.Normalize(1);
	
	FRotator desiredRotation = UKismetMathLibrary::MakeRotFromX(moveDirection);
	desiredRotation.Pitch = 0.f;
	desiredRotation.Roll = 0.f;

	FRotator newRotation = FMath::RInterpTo(GetActorRotation(), desiredRotation, GetWorld()->GetDeltaSeconds(), interpSpeed);
	SetActorRotation(newRotation);
}

void ABase_Pawn::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	UE_LOG(LogTemp, Display, TEXT("Move Completed for Request ID: %s"), *RequestID.ToString());
}

void ABase_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();

}

void ABase_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABase_Pawn::SelectActor_Implementation(const bool isSelected)
{
	selectedIndicator->SetHiddenInGame(!isSelected);
}

void ABase_Pawn::MoveToLocation_Implementation(const FVector targetLocation)
{
	moveTargetLocation = targetLocation + FVector(0,0,GetDefaultHalfHeight());
	bMoving = true;

	AAIController* pawnAIController = Cast<AAIController>(GetController());
	pawnAIController->MoveToLocation(targetLocation, minDistanceToTarget);
}

