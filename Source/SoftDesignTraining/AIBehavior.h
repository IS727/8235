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
    const float m_minWallSeparation = 150.0f;
    bool m_nextRandomTurnRight = true;

    bool m_running_away = false;
    bool m_trapped_by_player = false;

    bool m_avoidingWall = false;
    bool m_avoidingTrap = false;

    bool m_turnRight = false;
    float m_turnTime = 0.0f;
    float m_turnAngle = 0.0f;

    float m_turnDeltaTime = 0.0f;
    float m_turnDegree = 0.0f;
    FVector m_targetHeading;

    void MoveToTarget(APawn* const pawn, float& speed, FVector collectiblePos);
    void AvoidObstacle(APawn* const pawn, float deltaTime, float& speed);
    void KeepWallsAway(UWorld* world, APawn* const pawn, AIVision* vision);
    void ResetTurnState(FVector targetHeading, bool avoidingWall, bool avoidingTrap);
    void InitiateAvoidance(bool turnRight, float turnTime = 0.5f, float turnAngle = 90.0f);
    float GetPawnDegreesToVector(APawn* const pawn, const FVector vector) const;
    void TurnBack(APawn* const pawn, bool avoidingWall, bool avoidingTrap);
    void Dodge(APawn* const pawn, bool dodgeTowardsLeft);
    bool GetRandomTurnDirection();
};
