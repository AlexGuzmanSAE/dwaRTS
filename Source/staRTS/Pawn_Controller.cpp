// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Controller.h"

// Sets default values
APawn_Controller::APawn_Controller()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APawn_Controller::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APawn_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APawn_Controller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

