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

        if (DetectWordInFrontOfPawn(pawn))
            pawn->AddActorWorldRotation(FRotator(0.0f, 5.0f, 0.0f));
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

bool ASDTAIController::DetectWordInFrontOfPawn(APawn* const pawn) const
{
    const float lineTraceDistance = 300.0f;
    const FVector start(pawn->GetActorLocation());
    const FRotator pawnRotation(pawn->GetActorRotation());
    const FVector end(start + (pawnRotation.Vector() * lineTraceDistance));

    FHitResult hitResult;
    return GetWorld()->LineTraceSingleByObjectType(
        hitResult,
        start,
        end,
        ECC_WorldStatic
    );
}
