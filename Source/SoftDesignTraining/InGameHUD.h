// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "Components/WidgetComponent.h"
#include "DebugWidget.h"

#include "InGameHUD.generated.h"

/**
 *
 */
UCLASS()
class SOFTDESIGNTRAINING_API AInGameHUD : public AHUD
{
	GENERATED_BODY()


public:
	AInGameHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void ShowUpdatedCounters();

	UFUNCTION()
		int32 AddAI();

	UFUNCTION()
		void AddToDeathCounter(int32 Id);

	UFUNCTION()
		void AddToCollectibleCounter(int32 Id);

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> DebugWidgetClass;

private:

	UDebugWidget* DebugWidget;

	int32 AICounter = 0;

	struct AI
	{
		int32 Id;
		int32 DeathCounter;
		int32 CollectibleCounter;

	};

	struct AI AIArray[3];

};

