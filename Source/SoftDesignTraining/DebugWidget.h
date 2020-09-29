// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Runtime/UMG/Public/UMG.h"

#include "DebugWidget.generated.h"

/**
 *
 */
UCLASS()
class SOFTDESIGNTRAINING_API UDebugWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UDebugWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	void UpdateThingCounter(FString HUD);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTThing;

};
