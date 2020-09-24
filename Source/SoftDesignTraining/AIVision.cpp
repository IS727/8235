// Fill out your copyright notice in the Description page of Project Settings.


#include "AIVision.h"
#include "SDTUtils.h"
#include "PhysicsHelpers.h"
#include "SDTCollectible.h"

AIVision::AIVision()
{
}

AIVision::~AIVision()
{
}

// PUBLIC

bool AIVision::DetectWall(UWorld* world, APawn* const pawn, FVector& outObjectNormal, Dir direction)
{
    SetVisionParams(world, pawn, ECC_WorldStatic, direction);
    return DetectWallInDirection(outObjectNormal);
}

bool AIVision::DetectTrap(UWorld* world, APawn* const pawn, FVector& outObjectNormal)
{
    SetVisionParams(world, pawn, COLLISION_DEATH_OBJECT);
    return DetectObjectInDirection(outObjectNormal);
}

bool AIVision::DetectCollectible(UWorld* world, APawn* const pawn, FVector& outObjectNormal)
{
    SetVisionParams(world, pawn, COLLISION_COLLECTIBLE);
    return DetectObjectInDirection(outObjectNormal, true);
}

bool AIVision::DetectPlayer(UWorld* world, APawn* const pawn, FVector& outObjectNormal)
{
    SetVisionParams(world, pawn, COLLISION_PLAYER);
    return DetectObjectInDirection(outObjectNormal);
}

// PRIVATE

void AIVision::SetVisionParams(UWorld* world, APawn* const pawn, ECollisionChannel channel, Dir direction)
{
    m_world = world;
    m_pawn = pawn;
    m_direction = direction;
    m_channel = channel;
}

bool AIVision::DetectWallInDirection(FVector& outObjectNormal)
{
    float visionDist = 250.0f;
    FHitResult hitResult;
    FVector start(m_pawn->GetActorLocation());
    FVector dir(m_pawn->GetActorRotation().Vector());
    float rotation = 0.0f;

    if (m_direction == Dir::left) { rotation = -80.0f; visionDist = 300.0f; }
    else if (m_direction == Dir::right) { rotation = 80.0f; visionDist = 300.0f; }
    dir = FRotator(0.0f, rotation, 0.0f).RotateVector(dir) * visionDist;

    FVector end(start + dir);

    const bool foundWall = m_world->LineTraceSingleByObjectType(
        hitResult,
        start,
        end,
        m_channel
    );
    if (foundWall) outObjectNormal = FRotator(0.0f, -90.0f, 0.0f).RotateVector(hitResult.ImpactNormal.GetSafeNormal());
    return foundWall;
}

bool AIVision::DetectObjectInDirection(FVector& outObjectNormal, bool returnPos)
{
    const TArray<FOverlapResult> foundObjects = CollectVisibleObjects();
    const bool foundObject = foundObjects.Num() > 0;

    if (foundObject)
    {
        const FVector pos = foundObjects[0].GetActor()->GetActorLocation();
        outObjectNormal = returnPos ? pos : GetObjectNormal(pos);
    }
    return foundObject;
}

FVector AIVision::GetObjectNormal(FVector target)
{
    FHitResult hitResult;
    m_world->LineTraceSingleByObjectType(hitResult, m_pawn->GetActorLocation(), target, m_channel);
    return FRotator(0.0f, -90.0f, 0.0f).RotateVector(hitResult.ImpactNormal.GetSafeNormal());
}

TArray<FOverlapResult> AIVision::CollectVisibleObjects() const
{
    TArray<FOverlapResult> objects = CollectObjectsAround();

    objects = objects.FilterByPredicate([&](const FOverlapResult object)
    {
        const FVector start(m_pawn->GetActorLocation());
        const FVector end(object.GetActor()->GetActorLocation());
        TArray <FHitResult> hitData;

        // add obstacles
        FCollisionObjectQueryParams objectQueryParams = SDTUtils::GetAllObjectsQueryParams();
        objectQueryParams.RemoveObjectTypesToQuery(m_channel); // remove the target

        m_world->LineTraceMultiByObjectType(hitData, start, end, objectQueryParams);

        UPrimitiveComponent* objComponent = object.GetComponent();

        const float coneVisionDist = (m_channel == COLLISION_COLLECTIBLE || m_channel == COLLISION_PLAYER) ? 600.0f : 350.0f;
        bool objIsVisible = true;
        if (objComponent->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
        {
            AStaticMeshActor* collectible = dynamic_cast <AStaticMeshActor*>(object.GetActor());
            objIsVisible = collectible->GetStaticMeshComponent()->IsVisible();
        }

        return IsInsideCone(object.GetActor(), coneVisionDist) && objComponent->GetCollisionObjectType() == m_channel && hitData.Num() == 0 && objIsVisible;
    });
    return objects;
}

bool AIVision::IsInsideCone(AActor* targetActor, float visionDist) const
{
    const float visionAngle = 30.0f;

    // compute the cone direction vector
    FVector dir = m_pawn->GetActorForwardVector().GetSafeNormal();

    FVector const toTarget = targetActor->GetActorLocation() - m_pawn->GetActorLocation();
    const float acos = std::abs(std::acos(FVector::DotProduct(dir, toTarget.GetSafeNormal()))) * 180 / PI;
    const bool angleOk = acos < visionAngle;
    const bool distOk = toTarget.Size() < visionDist;
    return angleOk && distOk;
}

TArray<FOverlapResult> AIVision::CollectObjectsAround() const
{
    TArray<FOverlapResult> outResults;
    PhysicsHelpers physicHelper(m_world);
    physicHelper.SphereOverlap(m_pawn->GetActorLocation() + m_pawn->GetActorForwardVector() * 300.0f, 300.0f, outResults, false);
    return outResults;
}
