// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBehavior.h"

AIBehavior::AIBehavior()
{
}

AIBehavior::~AIBehavior()
{
}

// PUBLIC

void AIBehavior::HandleObstacles(UWorld* world, APawn* const pawn, float deltaTime, float& speed, AIVision* vision)
{
    const bool avoidingObstacle = m_avoidingWall || m_avoidingTrap;

    if (avoidingObstacle) AvoidObstacle(pawn, deltaTime, speed);
    else
    {
        // detect stuff
        FVector wallNormal, trapNormal, collectiblePos, playerPos;
        const bool blockingWallDetected = vision->DetectWall(world, pawn, wallNormal, AIVision::straight).Get<0>();
        const bool trapDetected = vision->DetectTrap(world, pawn, trapNormal);
        const bool collectibleDetected = vision->DetectCollectible(world, pawn, collectiblePos);
        const TTuple<bool, bool> playerDetection = vision->DetectPlayer(world, pawn, playerPos);
        const bool playerDetected = playerDetection.Get<0>();

        // decide what to do
        if (blockingWallDetected)
        {
            ResetTurnState(wallNormal, true, false);

            const bool rightClear = !vision->DetectWall(world, pawn, wallNormal, AIVision::right).Get<0>();
            const bool leftClear = !vision->DetectWall(world, pawn, wallNormal, AIVision::left).Get<0>();

            if (rightClear && leftClear) InitiateAvoidance(GetRandomTurnDirection());
            else if (rightClear) InitiateAvoidance(true);
            else if (leftClear) InitiateAvoidance(false);
            else HandleTrap(pawn);
        }
        else if (trapDetected) HandleTrap(pawn);
        else if (playerDetected)
        {
            const bool playerIsPoweredUp = playerDetection.Get<1>();
            if (playerIsPoweredUp) EscapePlayer(pawn, speed);
            else MoveToTarget(world, pawn, speed, playerPos, vision);
        }
        else if (collectibleDetected) MoveToTarget(world, pawn, speed, collectiblePos, vision);
        else  KeepWallsAway(world, pawn, vision);
    }
}

// PRIVATE

/**
 * Rotates the pawn towards the target position
 */
void AIBehavior::MoveToTarget(UWorld* world, APawn* const pawn, float& speed, FVector targetPos, AIVision* vision)
{
    ResetTrapStatus();
    Dodge(world, pawn, vision);
    FVector const toTarget((targetPos - pawn->GetActorLocation()).GetSafeNormal());
    const float degree = GetPawnDegreesToVector(pawn, toTarget);
    pawn->AddActorWorldRotation(FRotator(0.0f, degree, 0.0f));
}

/**
 * Performs a continuous turn of "m_turnAngle" degrees, in "m_turnTime" seconds
 * The turn is performed as long as either one of "m_avoidingWall" or "m_avoidingTrap" is true
 * 
 * @param speed the current speed of the pawn
 */
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

/**
 * Adjusts the pawn position and direction to remain away from any wall detected at the right or the left side of the pawn
 */
void AIBehavior::KeepWallsAway(UWorld* world, APawn* const pawn, AIVision* vision)
{
    ResetTrapStatus();

    FVector rightWallNormal, leftWallNormal;
    const TTuple<bool, float> rightWallStats = vision->DetectWall(world, pawn, rightWallNormal, AIVision::right);
    const TTuple<bool, float> leftWallStats = vision->DetectWall(world, pawn, leftWallNormal, AIVision::left);

    const bool rightWallDetected = rightWallStats.Get<0>();
    const bool leftWallDetected = leftWallStats.Get<0>();

    const float rightWallDist = rightWallStats.Get<1>();
    const float leftWallDist = leftWallStats.Get<1>();

    const float rightDegree = GetPawnDegreesToVector(pawn, rightWallNormal);
    const float leftDegree  = GetPawnDegreesToVector(pawn, leftWallNormal);

    // If the pawn's direction is at MOST this degree away from the wall normal, we correct it (in a single frame)
    // The higher it is the less realistik the pawn looks when we correct it
    const float maxDirError = 30.0f;

    // the speed we add to the pawn's vector opposite to the wall in order to keep the wall away
    const float wallEscapeSpeed = 0.1;

    if (rightWallDetected)
    {
        if (FMath::Abs(rightDegree) < maxDirError) pawn->AddActorWorldRotation(FRotator(0.0f, rightDegree, 0.0f));
        if (rightWallDist <= m_minWallSeparation) pawn->AddMovementInput(-pawn->GetActorRightVector().GetSafeNormal(), wallEscapeSpeed);
    }

    if (leftWallDetected)
    {
        if(FMath::Abs(leftDegree) < maxDirError) pawn->AddActorWorldRotation(FRotator(0.0f, leftDegree, 0.0f));
        if (leftWallDist <= m_minWallSeparation) pawn->AddMovementInput(pawn->GetActorRightVector().GetSafeNormal(), wallEscapeSpeed);
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

/*
 * Starts a turn
 * 
 * @param turnRight if true, the pawn will turn to the right
 * @param turnTime the amount of time (in seconds) the turn should be performed
 * @param turnAngle the turn angle
 */
void AIBehavior::InitiateAvoidance(bool turnRight, float turnTime, float turnAngle)
{
    m_turnRight = turnRight;
    m_turnTime = turnTime;
    m_turnAngle = turnAngle;
}

/*
 * @return the degrees between the pawn direction and the provided vector
 */
float AIBehavior::GetPawnDegreesToVector(APawn* const pawn, const FVector vector) const
{
    float degrees = FMath::RadiansToDegrees(std::acos(FVector::DotProduct(pawn->GetActorForwardVector(), vector)));
    return FVector::DotProduct(pawn->GetActorRightVector(), vector) < 0 ? -degrees : degrees;
}

/*
 * Makes the pawn perform a 180
 * 
 * @param avoidingWall if true, sets the pawn in the wall avoidance state
 * @param avoidingTrap if true, sets the pawn in the trap avoidance state
 */
void AIBehavior::TurnBack(APawn* const pawn, bool avoidingWall, bool avoidingTrap)
{
    ResetTurnState(FRotator(0.0f, -180.0f, 0.0f).RotateVector(pawn->GetActorForwardVector()), avoidingWall, avoidingTrap);
    InitiateAvoidance(true, 0.2f, 180.0f);
}

/*
 * Makes the pawn dodge a side wall
 *
 * @param dodgeTowardsLeft if true, the pawn will dodge via the left
 */
void AIBehavior::Dodge(UWorld* world, APawn* const pawn, AIVision* vision)
{
    FVector leftWallNormal, rightWallNormal;
    const bool rightAngleBlocked = vision->DetectWall(world, pawn, rightWallNormal, AIVision::angleRight).Get<0>();
    const bool leftAngleBlocked = vision->DetectWall(world, pawn, leftWallNormal, AIVision::angleLeft).Get<0>();
    const bool nonBlockingWallDetected = rightAngleBlocked || leftAngleBlocked;

    if (nonBlockingWallDetected)
    {
        FVector escape = pawn->GetActorRightVector().GetSafeNormal();
        escape = rightAngleBlocked ? -escape : escape;
        pawn->AddMovementInput(escape, 0.2);
    }
}

/*
 * Returns the next direction the pawn should turn when blocked by a wall if both left and right are free
 *
 * @return true if the pawn should turn to the right. false otherwise
 */
bool AIBehavior::GetRandomTurnDirection()
{
    const bool nextDirection = m_nextRandomTurnRight;
    m_nextRandomTurnRight = !m_nextRandomTurnRight;
    return nextDirection;
}

void AIBehavior::ResetTrapStatus()
{
    m_running_away = false;
    m_trapped_by_player = false;
}

/*
 * Makes the pawn get away from the player
 */
void AIBehavior::EscapePlayer(APawn* const pawn, float& speed)
{
    m_running_away = true;
    if (m_trapped_by_player) speed = 0;
    else TurnBack(pawn, false, true);
}

/*
 * Makes the pawn move away from a blocking object in front of him
 */
void AIBehavior::HandleTrap(APawn* const pawn)
{
    if (m_running_away) m_trapped_by_player = true;
    TurnBack(pawn, false, true);
}
