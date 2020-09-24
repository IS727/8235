// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIVision.h"

/**
 * 
 */
class SOFTDESIGNTRAINING_API AIBehavior
{
public:
    AIBehavior();
    ~AIBehavior();

    void HandleObstacles(UWorld* world, APawn* const pawn, float deltaTime, float& speed, AIVision* vision);

private:
    bool m_avoidingWall = false;
    bool m_avoidingTrap = false;

    bool m_turnRight = false;
    float m_turnTime = 0.0f;
    float m_turnAngle = 0.0f;

    float m_turnDeltaTime = 0.0f;
    float m_turnDegree = 0.0f;
    FVector m_targetHeading;

    void MoveToCollectible(APawn* const pawn, float& speed, FVector collectiblePos);
    void AvoidObstacle(APawn* const pawn, float deltaTime, float& speed);
    void ResetTurnState(FVector targetHeading, bool avoidingWall, bool avoidingTrap);
    void InitiateAvoidance(bool turnRight, float turnTime, float turnAngle);
};
