// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Controller2.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


APawn_Controller2::APawn_Controller2()
{
	PrimaryActorTick.bCanEverTick = true;

	capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	RootComponent = capsuleComponent;

	springComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Component"));
	springComponent->SetupAttachment(RootComponent);


	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	cameraComponent->SetupAttachment(springComponent);
	cameraComponent->SetupAttachment(springComponent, USpringArmComponent::SocketName);
	cameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);

	floatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
}

void APawn_Controller2::BeginPlay()
{
	Super::BeginPlay();
	
}

void APawn_Controller2::Move(const FInputActionValue& value)
{
	const FVector2D movementVector = value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator controllerRotation = Controller->GetControlRotation();
		const FRotator yawRotation(0, controllerRotation.Yaw, 0);

		const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(forwardDirection, movementVector.Y * 5);
		AddMovementInput(rightDirection, movementVector.X * 5);
	}
}

void APawn_Controller2::Zoom(const FInputActionValue& value)
{
	const float zoomValue = value.Get<float>();
	if (Controller != nullptr)
	{
		float desiredOrthographicSize = cameraComponent->OrthoWidth - zoomValue * cameraZoomSpeed;
		desiredOrthographicSize = FMath::Clamp(desiredOrthographicSize, minCameraZoom, maxCameraZoom);
		cameraComponent->OrthoWidth = desiredOrthographicSize;
	}
}

void APawn_Controller2::EdgeScroll()
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (!playerController) return;

	float mouseX{}, mouseY{};

	if (playerController->GetMousePosition(mouseX, mouseY))
	{
		FVector2D viewportSize{};
		if (GEngine &&  GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(viewportSize);

			float edgeScrollThreshold = 10.f;
			FVector2D movementInput = FVector2D::ZeroVector;

			if (mouseX < edgeScrollThreshold)
			{
				//left
				movementInput.X = -1.f;
			}
			if (mouseX > viewportSize.X - edgeScrollThreshold)
			{
				//right
				movementInput.X = 1.f;
			}
			if (mouseY < edgeScrollThreshold)
			{
				//up
				movementInput.Y = 1.f;
			}
			if (mouseY > viewportSize.Y - edgeScrollThreshold)
			{
				//down
				movementInput.Y = -1.f;	
			}

			if (movementInput.IsZero())
			{
				Move(FInputActionValue(movementInput));
			}
		}
	}
}

void APawn_Controller2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EdgeScroll();
}

void APawn_Controller2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent) )
	{
		EnhancedInputComponent->BindAction(inputAction, ETriggerEvent::Triggered, this, &APawn_Controller2::Move);

		EnhancedInputComponent->BindAction(zoomAction, ETriggerEvent::Triggered, this, &APawn_Controller2::Zoom);
	}
}


