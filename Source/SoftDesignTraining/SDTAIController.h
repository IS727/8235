// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController.h"
#include "AIBehavior.h"

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

private:
    // ACCELERATION AND SPEED
    float acceleration = 0.3;
    float speed = 0;
    const float speedLimit = 0.6;

    // TURN and obstacle avoidance
    AIBehavior behavior;
    AIVision vision;

    virtual void UpdateSpeed(APawn* const pawn, float deltaTime);
};
