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
        HandleObstacles(pawn, deltaTime);
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

void ASDTAIController::HandleObstacles(APawn* const pawn, float deltaTime)
{
    const bool newWall = DetectWallInFrontOfPawn(pawn) && !turning;
    AvoidWall(pawn, deltaTime, newWall, true);
}

/*
 * Avoid a wall with a 90 degrees turn. Turning rate is around 45 degrees per 0.25 seconds
 * 
 * @param init If "true", initializes a new turn
 * @param right If "true", turns to the right. Otherwise, turns to the left
 */
void ASDTAIController::AvoidWall(APawn* const pawn, float deltaTime, bool init, bool right)
{
    const float turnMaxDegree = 90.0f;
    const float turnTime = 0.5f;
    const float direction = right ? 1.0 : -1.0;
    const float reachedHeading = FVector::Parallel(pawn->GetActorForwardVector(), newHeading);

    if (init)
    {
        turning = true;
        turnDeltaTime = 0.0f;
        turnDegree = 0.0f;
    }
    else
    {
        turning = turning? !reachedHeading : turning;
        if (turning)
        {
            // slow down for the turn
            speed = 0.3;

            // compute degree
            const float expectedDegree = turnDeltaTime * (turnMaxDegree / turnTime);
            const float degree = abs(expectedDegree - turnDegree);
            turnDeltaTime += deltaTime;
            turnDegree += degree;

            // turn
            pawn->AddActorWorldRotation(FRotator(0.0f, direction * degree, 0.0f));
        }
    }
}

bool ASDTAIController::DetectWallInFrontOfPawn(APawn* const pawn)
{
    const float lineTraceDistance = 150.0f;
    const FVector start(pawn->GetActorLocation());
    const FRotator pawnRotation(pawn->GetActorRotation());
    const FVector end(start + (pawnRotation.Vector() * lineTraceDistance));

    FHitResult hitResult;
    const bool wallDetected = GetWorld()->LineTraceSingleByObjectType(
        hitResult,
        start,
        end,
        ECC_WorldStatic
    );
    if (!turning && wallDetected) newHeading = FRotator(0.0f, -90.0f, 0.0f).RotateVector(hitResult.ImpactNormal.GetSafeNormal());
    return wallDetected;
}
