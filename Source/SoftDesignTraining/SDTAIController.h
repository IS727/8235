// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController.h"
#include "AIBehavior.h"

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
	/** The constant movement acceleration that the AI character undergo every tick in cm^2/tick  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SimpleDisplay, Category = Movement)
	float acceleration = 0.3;
	/** The current speed that the AI character is going at in cm/tick  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SimpleDisplay, Category = Movement)
	float speed = 0;
	/** The maximum speed that the AI character can achieve in cm/tick  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SimpleDisplay, Category = Movement)
	float speedLimit = 0.6;

	/** The angle of the cone with which the character can see traps, collectibles and other characters in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SimpleDisplay, Category = Vision)
	float m_visionAngle = 25.0f;
	/** The depth of the cone that the character uses to see walls, and traps in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SimpleDisplay, Category = Vision)
	float m_wallVisionDist = 350.0f; 
	/** The depth of the cone that the character uses to see collectibles and players in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SimpleDisplay, Category = Vision)
	float m_lineVisionDist = 600.0f;
private:

    // TURN and obstacle avoidance
    AIBehavior behavior;
    AIVision vision =  AIVision(m_visionAngle, m_wallVisionDist, m_lineVisionDist);

    virtual void UpdateSpeed(APawn* const pawn, float deltaTime);
};
