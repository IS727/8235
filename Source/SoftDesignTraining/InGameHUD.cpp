// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUD.h"

AInGameHUD::AInGameHUD()
{


}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (DebugWidgetClass)
	{
		DebugWidget = CreateWidget<UDebugWidget>(GetWorld(), DebugWidgetClass);
		if (DebugWidget)
		{
			DebugWidget->AddToViewport();
		}
	}
}

void AInGameHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AInGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AInGameHUD::UpdateThingCounter()
{

	FString HUD = " Time elapsed (in seconds) : " + FString::FromInt(GetWorld()->GetAudioTimeSeconds()) + "\r";
	FString SId;
	FString SDeathCounter;
	FString SCollectibleCounter;

	GetWorld()->GetAudioTimeSeconds();

	for (int i = 0; i < 3; i++)
	{
		if (AIArray[i].Id != 0)
		{
			SId = " Id : " + FString::FromInt(AIArray[i].Id);
			SDeathCounter = " DeathCounter : " + FString::FromInt(AIArray[i].DeathCounter);
			SCollectibleCounter = " CollectibleCounter : " + FString::FromInt(AIArray[i].CollectibleCounter);

			HUD = HUD + SId + SDeathCounter + SCollectibleCounter + "\r";
		}
	}

	if (DebugWidget)
	{
		DebugWidget->UpdateThingCounter(HUD);
	}

}

int32 AInGameHUD::AddAI()
{

	AICounter = AICounter + 1;
	AIArray[AICounter].Id = AICounter;
	AIArray[AICounter].DeathCounter = 0;
	AIArray[AICounter].CollectibleCounter = 0;

	UE_LOG(LogTemp, Warning, TEXT("HUD log, %d"), AIArray[AICounter].Id);

	return AICounter;

}

void AInGameHUD::AddToDeathCounter(int32 Id)
{
	AIArray[Id].DeathCounter = AIArray[Id].DeathCounter + 1;
	UpdateThingCounter();
}

void AInGameHUD::AddToCollectibleCounter(int32 Id)
{
	AIArray[Id].CollectibleCounter = AIArray[Id].CollectibleCounter + 1;
	UpdateThingCounter();
}