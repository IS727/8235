// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugWidget.h"

UDebugWidget::UDebugWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();

}


// affiche le HUD avec toutes les informations contenues dans le tableau AIArray
void UDebugWidget::UpdateThingCounter(FString HUD)
{

	if (TXTThing)
	{
		if (TXTThing->Visibility == ESlateVisibility::Hidden)
		{
			TXTThing->SetVisibility(ESlateVisibility::Visible);
		}
		TXTThing->SetText(FText::FromString(HUD));

	}

}
