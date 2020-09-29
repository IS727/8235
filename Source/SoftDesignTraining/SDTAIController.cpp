// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "DrawDebugHelpers.h"


void ASDTAIController::Tick(float deltaTime)
{
    APawn* const pawn = GetPawn();
    if (pawn)
    {
        UpdateSpeed(pawn, deltaTime);
        behavior.HandleObstacles(GetWorld(), pawn, deltaTime, speed, &vision);
    }
}

void ASDTAIController::UpdateSpeed(APawn* const pawn, float deltaTime)
{
    // compute speed
    speed += acceleration * deltaTime;
    speed = speed > speedLimit ? speedLimit : speed;

    // update pawn
    FVector dir = pawn->GetActorForwardVector();
    pawn->AddMovementInput(dir, speed);
}
