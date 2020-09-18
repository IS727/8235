// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()
public:
    virtual void Tick(float deltaTime) override;

    // ACCELERATION AND SPEED
    float acceleration = 0.3;
    float speed = 0;
    const float speedLimit = 0.6;

    // TURN
    bool turning = false;
    float turnDeltaTime = 0.0f;
    float turnDegree = 0.0f;
    FVector newHeading;

private:
    virtual void UpdateSpeed(APawn* const pawn, float deltaTime);
    virtual void HandleObstacles(APawn* const pawn, float deltaTime);
    virtual void AvoidWall(APawn* const pawn, float deltaTime, bool init, bool right);
    virtual bool DetectWallInFrontOfPawn(APawn* const pawn);
};
