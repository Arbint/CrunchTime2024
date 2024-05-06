// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/MenuPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SpawnUI();
	}
}

void AMenuPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SpawnUI();
}

void AMenuPlayerController::SpawnUI()
{
	if (IsLocalPlayerController() && MenuToSpawn)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(this, MenuToSpawn);
		Widget->AddToViewport();
		SetShowMouseCursor(true);
	}
}
