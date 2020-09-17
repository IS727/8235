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

    float acceleration = 0.3;
    float speed = 0;
    const float speedLimit = 0.6;

private:
    virtual void UpdateSpeed(APawn* const pawn, float deltaTime);
    virtual bool DetectWordInFrontOfPawn(APawn* const pawn) const;
};
