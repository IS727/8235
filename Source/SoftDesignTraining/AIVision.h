// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SOFTDESIGNTRAINING_API AIVision
{
public:
    AIVision();
    ~AIVision();

    enum Dir { left, right, straight };

    bool DetectWall(UWorld* world, APawn* const pawn, FVector& outObjectNormal, Dir direction = Dir::straight);
    bool DetectTrap(UWorld* world, APawn* const pawn, FVector& outObjectNormal);
    bool DetectCollectible(UWorld* world, APawn* const pawn, FVector& outObjectNormal);
    bool DetectPlayer(UWorld* world, APawn* const pawn, FVector& outObjectNormal);

private:

    UWorld* m_world;
    APawn* m_pawn;
    ECollisionChannel m_channel;
    Dir m_direction;

    void SetVisionParams(UWorld* world, APawn* const pawn, ECollisionChannel channel, Dir direction = Dir::straight);

    bool DetectObjectInDirection(FVector& ObjectNormal, bool returnPos = false);
    bool DetectWallInDirection(FVector& outObjectNormal);
    FVector GetObjectNormal(FVector target);
    bool IsInsideCone(AActor* targetActor, float visionDist = 150.0f) const;
    TArray<FOverlapResult> CollectVisibleObjects() const;
    TArray<FOverlapResult> CollectObjectsAround() const;
};
