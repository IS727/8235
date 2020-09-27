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

    enum Dir { left, right, straight, angleLeft, angleRight };

    TTuple<bool, float> DetectWall(UWorld* world, APawn* const pawn, FVector& outObjectNormal, Dir direction = Dir::straight);
    bool DetectTrap(UWorld* world, APawn* const pawn, FVector& outObjectNormal);
    bool DetectCollectible(UWorld* world, APawn* const pawn, FVector& outCollectiblePos);
    TTuple<bool, bool> DetectPlayer(UWorld* world, APawn* const pawn, FVector& outPlayerPos);

    const float m_visionAngle = 25.0f;
    float m_coneVisionDist = 350.0f;

private:

    UWorld* m_world;
    APawn* m_pawn;
    ECollisionChannel m_channel;
    Dir m_direction;

    void SetVisionParams(UWorld* world, APawn* const pawn, ECollisionChannel channel, float coneVisionDist = 350.0f, Dir direction = Dir::straight);

    TTuple<bool, AActor*> DetectObjectInDirection(FVector& ObjectNormal, bool returnPos = false);
    TTuple<bool, float> DetectWallInDirection(FVector& outObjectNormal);
    FVector GetObjectNormal(FVector target);
    bool ObjectIsVisible(const FOverlapResult object, const TArray <FHitResult> hitData, bool objIsHidden) const;
    bool IsInsideCone(AActor* targetActor) const;
    TArray<FOverlapResult> CollectVisibleObjects() const;
    TArray<FOverlapResult> CollectObjectsAround() const;
};
