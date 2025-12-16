#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ISelectable.h"
#include "Navegable_Interface.h"
#include "Base_Pawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UFloatingPawnMovement;
struct FPathFollowingResult;
struct FAIRequestID;
class AAIController;

UCLASS()
class STARTS_API ABase_Pawn : public APawn, public IISelectable, public INavegable_Interface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UCapsuleComponent> capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<USkeletalMeshComponent> skeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UFloatingPawnMovement> floatPawnMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UStaticMeshComponent> selectedIndicator;;

public:
	
	ABase_Pawn();

protected:
	
	virtual void BeginPlay() override;

	void Move();

	FVector moveTargetLocation = FVector::ZeroVector;
	bool bMoving = false;
	float minDistanceToTarget = 50.f;
	float interpSpeed = 5.f;


public:	
	//DEBUG
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	//


	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SelectActor_Implementation(const bool isSelected);
	void MoveToLocation_Implementation(const FVector targetLocation) override;
};
