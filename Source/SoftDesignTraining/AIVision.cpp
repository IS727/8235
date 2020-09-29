// Fill out your copyright notice in the Description page of Project Settings.


#include "AIVision.h"
#include "SDTUtils.h"
#include "PhysicsHelpers.h"
#include "SDTCollectible.h"
#include <SoftDesignTraining\SoftDesignTrainingMainCharacter.h>

AIVision::AIVision() {

}

AIVision::AIVision(float visionAngle, float wallVisionDist, float lineVisionDist)
{
	m_visionAngle = visionAngle;
	m_wallVisionDist = wallVisionDist;
	m_lineVisionDist = lineVisionDist;
}

AIVision::~AIVision()
{
}

// PUBLIC

/**
 * Detects a wall in a specific direction by using a raycast
 *
 * @param outObjectNormal the wall normal vector
 * @return a tuple with:
 *  1-> true if a wall is detected
 *  2-> the distance from the pawn to the wall
 */
TTuple<bool, float> AIVision::DetectWall(UWorld* world, APawn* const pawn, FVector& outObjectNormal, Dir direction)
{
    SetVisionParams(world, pawn, ECC_WorldStatic, m_wallVisionDist, direction);
    return DetectWallInDirection(outObjectNormal);
}

/**
 * Detects a deadly trap in front of the pawn
 *
 * @param outObjectNormal the trap normal vector
 * @return true if a trap is detected
 */
bool AIVision::DetectTrap(UWorld* world, APawn* const pawn, FVector& outObjectNormal)
{
    SetVisionParams(world, pawn, COLLISION_DEATH_OBJECT, m_wallVisionDist);
    return DetectObjectInDirection(outObjectNormal).Get<0>();
}

/**
 * Detects the closest collectible in front of the pawn
 *
 * @param outCollectiblePos the collectible position
 * @return true if a collectible is detected
 */
bool AIVision::DetectCollectible(UWorld* world, APawn* const pawn, FVector& outCollectiblePos)
{
    SetVisionParams(world, pawn, COLLISION_COLLECTIBLE, m_lineVisionDist);
    return DetectObjectInDirection(outCollectiblePos, true).Get<0>();
}

/**
 * Detects the player in front of the pawn
 *
 * @param outPlayerPos the player position
 * @return a tuple with:
 *  1-> true if a player is detected
 *  2-> true if the player is powered up
 */
TTuple<bool, bool> AIVision::DetectPlayer(UWorld* world, APawn* const pawn, FVector& outPlayerPos)
{
    SetVisionParams(world, pawn, COLLISION_PLAYER, m_lineVisionDist);
    TTuple<bool, AActor*> res = DetectObjectInDirection(outPlayerPos, true);
    const bool playerFound = res.Get<0>();
    bool playerIsPoweredUp = false;
    if (playerFound)
    {
        ASoftDesignTrainingMainCharacter* player = dynamic_cast <ASoftDesignTrainingMainCharacter*>(res.Get<1>());
        playerIsPoweredUp = player->IsPoweredUp();
    }
    return TTuple<bool, bool>(playerFound, playerIsPoweredUp);
}

// PRIVATE

/**
 * Sets all parameters required to perform a detection
 */
void AIVision::SetVisionParams(UWorld* world, APawn* const pawn, ECollisionChannel channel, float coneVisionDist, Dir direction)
{
    m_world = world;
    m_pawn = pawn;
    m_direction = direction;
    m_channel = channel;
    m_coneVisionDist = coneVisionDist;
}

/**
 * Detects a wall by performing a raycast according to the parameters set in "AIVision::SetVisionParams"
 *
 * @param outObjectNormal see "AIVision::DetectWall"
 * @return see "AIVision::DetectWall"
 */
TTuple<bool, float> AIVision::DetectWallInDirection(FVector& outObjectNormal) const
{
    float visionDist = 250.0f;
    FHitResult hitResult;
    FVector start(m_pawn->GetActorLocation());
    FVector dir(m_pawn->GetActorRotation().Vector());
    float rotation = 0.0f;

    if (m_direction == Dir::left) { rotation = -80.0f; visionDist = 300.0f; }
    else if (m_direction == Dir::right) { rotation = 80.0f; visionDist = 300.0f; }
    else if (m_direction == Dir::angleRight) rotation = m_visionAngle;
    else if (m_direction == Dir::angleLeft) rotation = -m_visionAngle;
    dir = FRotator(0.0f, rotation, 0.0f).RotateVector(dir) * visionDist;

    FVector end(start + dir);

    const bool foundWall = m_world->LineTraceSingleByObjectType(hitResult, start, end, m_channel);
    if (foundWall) outObjectNormal = FRotator(0.0f, -90.0f, 0.0f).RotateVector(hitResult.ImpactNormal.GetSafeNormal());
    return TTuple<bool, float>(foundWall, hitResult.Distance);
}

/**
 * Detects an object by using a sphere overlap according to the parameters set in "AIVision::SetVisionParams"
 *
 * @param outObjectNormal see "AIVision::DetectWall"
 * @param returnPos true if "outObjectNormal" should contain the object position instead of the normal
 * @return see "AIVision::DetectWall"
 */
TTuple<bool, AActor*> AIVision::DetectObjectInDirection(FVector& outObjectNormal, bool returnPos) const
{
    const TArray<FOverlapResult> foundObjects = CollectVisibleObjects();
    const bool foundObject = foundObjects.Num() > 0;
    AActor* objActor;

    if (foundObject)
    {
        objActor = foundObjects[0].GetActor();
        const FVector pos = objActor->GetActorLocation();
        outObjectNormal = returnPos ? pos : GetObjectNormal(pos);
    }
    return TTuple<bool, AActor*>(foundObject, objActor);
}

/**
 * @return the provided target normal vector
 */
FVector AIVision::GetObjectNormal(FVector target) const
{
    FHitResult hitResult;
    m_world->LineTraceSingleByObjectType(hitResult, m_pawn->GetActorLocation(), target, m_channel);
    return FRotator(0.0f, -90.0f, 0.0f).RotateVector(hitResult.ImpactNormal.GetSafeNormal());
}

/**
 * Collects all objects in front of the pawn according to the parameters set in "AIVision::SetVisionParams"
 */
TArray<FOverlapResult> AIVision::CollectVisibleObjects() const
{
    TArray<FOverlapResult> objects = CollectObjectsAround();

    objects = objects.FilterByPredicate([&](const FOverlapResult object)
    {
        const FVector start(m_pawn->GetActorLocation());
        const FVector end(object.GetActor()->GetActorLocation());
        TArray <FHitResult> hitData;

        // add all obstacles
        FCollisionObjectQueryParams objectQueryParams = SDTUtils::GetAllObjectsQueryParams();
        objectQueryParams.RemoveObjectTypesToQuery(m_channel); // remove the target

        m_world->LineTraceMultiByObjectType(hitData, start, end, objectQueryParams);
        hitData = RemoveHiddenObjectsFromHitResults(hitData);

        UPrimitiveComponent* objComponent = object.GetComponent();

        // check if object is not hidden
        return ObjectIsVisibleToPawn(object, hitData, !ObjectIsVisible(objComponent, object.GetActor()));
    });
    return objects;
}

/**
 * Removes all hidden objects from a hit result
 */
TArray <FHitResult> AIVision::RemoveHiddenObjectsFromHitResults(TArray <FHitResult> hitData) const
{
    return hitData.FilterByPredicate([&](const FHitResult obj) {
        return ObjectIsVisible(obj.GetComponent(), obj.GetActor());
    });
}

/**
 * @return true if an object is visible to the world
 */
bool AIVision::ObjectIsVisible(UPrimitiveComponent* targetComponent, AActor* targetActor) const
{
    bool objIsVisible = true;
    if (targetComponent->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
    {
        AStaticMeshActor* collectible = dynamic_cast <AStaticMeshActor*>(targetActor);
        objIsVisible = collectible->GetStaticMeshComponent()->IsVisible();
    }
    return objIsVisible;
}

/**
 * @return true if an object is visible to the pawn
 */
bool AIVision::ObjectIsVisibleToPawn(const FOverlapResult object, const TArray <FHitResult> hitData, bool objIsHidden) const
{
    const bool objIsInsideCone = IsInsideCone(object.GetActor());
    const bool objIsRelevant = object.GetComponent()->GetCollisionObjectType() == m_channel;
    const bool objIsClearToReach = hitData.Num() == 0;
    return objIsInsideCone && objIsRelevant && objIsClearToReach && !objIsHidden;
}

/**
 * @return true if an object is inside the pawn vision cone
 */
bool AIVision::IsInsideCone(AActor* targetActor) const
{
    const FVector dir = m_pawn->GetActorForwardVector().GetSafeNormal();
    const FVector toTarget = targetActor->GetActorLocation() - m_pawn->GetActorLocation();
    const float acos = FMath::RadiansToDegrees(std::acos(FVector::DotProduct(dir, toTarget.GetSafeNormal())));
    const bool angleOk = acos < m_visionAngle;
    const bool distOk = toTarget.Size() < m_coneVisionDist;
    return angleOk && distOk;
}

/**
 * Collects all objects around the pawn
 */
TArray<FOverlapResult> AIVision::CollectObjectsAround() const
{
    TArray<FOverlapResult> outResults;
    PhysicsHelpers physicHelper(m_world);
    physicHelper.SphereOverlap(m_pawn->GetActorLocation() + m_pawn->GetActorForwardVector() * 300.0f, 300.0f, outResults, false);
    return outResults;
}
