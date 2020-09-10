// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

void ASDTAIController::Tick(float deltaTime)
{
    APawn* const pawn = GetPawn();

    if (pawn)
    {
        speedVector = pawn->GetActorForwardVector().GetSafeNormal();
        pawn->AddMovementInput(speedVector);
    }

}




