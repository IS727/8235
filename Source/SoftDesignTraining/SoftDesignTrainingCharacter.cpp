// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingCharacter.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "SDTUtils.h"
#include "DrawDebugHelpers.h"
#include "SDTCollectible.h"
#include "InGameHUD.h"

ASoftDesignTrainingCharacter::ASoftDesignTrainingCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
}

void ASoftDesignTrainingCharacter::BeginPlay()
{
    Super::BeginPlay();

    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASoftDesignTrainingCharacter::OnBeginOverlap);
    m_StartingPosition = GetActorLocation();

    // Get the HUD variable from InGameHUD
    InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
    
    // Init the identifiers of all character in the HUD
    if (InGameHUD)
    {
        if (!Cast<ASoftDesignTrainingMainCharacter>(this)) // prevent the player character to act in the HUD (if he does, the game will crash eventually)
        {
            if (HUDId == 0)
            {
                HUDId = InGameHUD->AddAI();
            }
        }
    }

}

void ASoftDesignTrainingCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherComponent->GetCollisionObjectType() == COLLISION_DEATH_OBJECT)
    {
        SetActorLocation(m_StartingPosition);

        // add a death to the score of the character and update the HUD to show it
        if (!Cast<ASoftDesignTrainingMainCharacter>(this)) 
        {
            if (InGameHUD)
            {
                InGameHUD->AddToDeathCounter(HUDId);
                InGameHUD->ShowUpdatedCounters();
            }
        }
    }
    else if(ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(OtherActor))
    {
        if (!collectibleActor->IsOnCooldown())
        {
            OnCollectPowerUp();
        }

        collectibleActor->Collect();

        // add a collectible to the score of the character and update the HUD to show it
        if (!Cast<ASoftDesignTrainingMainCharacter>(this))
        {
            if (InGameHUD)
            {
                InGameHUD->AddToCollectibleCounter(HUDId);
                InGameHUD->ShowUpdatedCounters();
            }
        }

    }
    else if (ASoftDesignTrainingMainCharacter* mainCharacter = Cast<ASoftDesignTrainingMainCharacter>(OtherActor))
    {
        /* There is a bug there (without our code), as this function seems to be called twice when the player character collide with an AI character ; you may use the UE_LOGS tu test it out

            if the player character is powered up, the ispoweredup log will fire twice
            if the player character is not powered up AND there is a collectible on the start position of the playercharacter, the isNOTpoweredup log will fire once and the ispoweredup log will fire once
            if the player character is not powered up AND there is no collectible on the start position of the playercharacter, the isNOTpoweredup log will fire twice

            the result with our code is that the deathounter will add more death than it should
        */

        if (mainCharacter->IsPoweredUp())
        {
            SetActorLocation(m_StartingPosition);

            
            UE_LOG(LogTemp, Warning, TEXT("ispoweredup log %s"), (mainCharacter->IsPoweredUp() ? TEXT("True") : TEXT("False")));

            if (!Cast<ASoftDesignTrainingMainCharacter>(this))
            {
                if (InGameHUD)
                {
                    InGameHUD->AddToDeathCounter(HUDId);
                    InGameHUD->ShowUpdatedCounters();
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("isNOTpoweredup log %s"), (mainCharacter->IsPoweredUp() ? TEXT("True") : TEXT("False")));
        }

        
    }
}
