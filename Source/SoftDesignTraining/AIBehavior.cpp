// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBehavior.h"

AIBehavior::AIBehavior()
{
}

AIBehavior::~AIBehavior()
{
}

void AIBehavior::HandleObstacles(UWorld* world, APawn* const pawn, float deltaTime, float& speed, AIVision* vision)
{
    const bool avoidingObstacle = m_avoidingWall || m_avoidingTrap;

    if (avoidingObstacle) AvoidObstacle(pawn, deltaTime, speed);
    else
    {
        // detect stuff
        FVector wallNormal, trapNormal, collectiblePos, playerNormal;
        const bool wallDetected = vision->DetectWall(world, pawn, wallNormal, AIVision::straight);
        const bool trapDetected = vision->DetectTrap(world, pawn, trapNormal);
        const bool collectibleDetected = vision->DetectCollectible(world, pawn, collectiblePos);
        const bool playerDetected = vision->DetectPlayer(world, pawn, collectiblePos);
        
        // decide what to do
        if (wallDetected)
        {
            ResetTurnState(wallNormal, true, false);

            const bool rightClear = !vision->DetectWall(world, pawn, wallNormal, AIVision::right);
            const bool leftClear = !vision->DetectWall(world, pawn, wallNormal, AIVision::left);

            if (rightClear && leftClear)
            {
                const bool randomDirection = FMath::RandRange(0, 1) == 0;
                InitiateAvoidance(randomDirection, 0.5f, 90.0f);
            }
            else if (rightClear) { InitiateAvoidance(true, 0.5f, 90.0f);}
            else if (leftClear) { InitiateAvoidance(false, 0.5f, 90.0f);}
            else
            {
                ResetTurnState(FRotator(0.0f, 180.0f, 0.0f).RotateVector(pawn->GetActorForwardVector()), true, false);
                InitiateAvoidance(true, 0.2f, 180.0f);
            }
        }
        else if (trapDetected)
        {
            ResetTurnState(FRotator(0.0f, -180.0f, 0.0f).RotateVector(pawn->GetActorForwardVector()), false, true);
            InitiateAvoidance(true, 0.2f, 180.0f);
        }
        else if (playerDetected)
        {
            // ...
        }
        else if (collectibleDetected)
        {
            MoveToCollectible(pawn, speed, collectiblePos, deltaTime);
        }
    }
}

void AIBehavior::MoveToCollectible(APawn* const pawn, float& speed, FVector collectiblePos)
{
    FVector const dir(pawn->GetActorForwardVector());
    FVector const toTarget((collectiblePos - pawn->GetActorLocation()).GetSafeNormal());
    float degree = std::acos(FVector::DotProduct(dir, toTarget)) * 180 / PI;
    const bool left = FVector::DotProduct(pawn->GetActorRightVector(), toTarget) < 0;
    degree = left ? -degree : degree;

    pawn->AddActorWorldRotation(FRotator(0.0f, degree, 0.0f));
}

void AIBehavior::AvoidObstacle(APawn* const pawn, float deltaTime, float& speed)
{
    const float direction = m_turnRight ? 1.0 : -1.0;
    const bool parallelToHeading = FVector::Parallel(pawn->GetActorForwardVector(), m_targetHeading);
    const bool headedRight = FVector::DotProduct(pawn->GetActorForwardVector(), m_targetHeading) > 0;
    const bool headedToHeading = m_turnRight ? headedRight : !headedRight;
    const bool timelapsed = m_turnDeltaTime >= m_turnTime;
    const bool reachedHeading = parallelToHeading && headedToHeading;
    const bool turnCompleted = timelapsed || reachedHeading;

    m_avoidingWall = m_avoidingWall ? !turnCompleted : m_avoidingWall;
    m_avoidingTrap = m_avoidingTrap ? !turnCompleted : m_avoidingTrap;

    if (m_avoidingWall || m_avoidingTrap)
    {
        // slow down for the turn
        speed = 0.3;

        // compute degree
        const float expectedDegree = m_turnDeltaTime * (m_turnAngle / m_turnTime);
        const float degree = abs(expectedDegree - m_turnDegree);
        m_turnDeltaTime += deltaTime;
        m_turnDegree += degree;

        // turn
        pawn->AddActorWorldRotation(FRotator(0.0f, direction * degree, 0.0f));
    }
}

void AIBehavior::ResetTurnState(FVector targetHeading, bool avoidingWall, bool avoidingTrap)
{
    m_avoidingWall = avoidingWall;
    m_avoidingTrap = avoidingTrap;
    m_targetHeading = targetHeading;
    m_turnDeltaTime = 0.0f;
    m_turnDegree = 0.0f;
    m_turnTime = 0.0f;
    m_turnAngle = 0.0f;
}

void AIBehavior::InitiateAvoidance(bool turnRight, float turnTime, float turnAngle)
{
    m_turnRight = turnRight;
    m_turnTime = turnTime;
    m_turnAngle = turnAngle;
}
