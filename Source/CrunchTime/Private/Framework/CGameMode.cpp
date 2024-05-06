// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameMode.h"
#include "Framework/CPlayerState.h"
#include "GameFramework/Character.h"

ACGameMode::ACGameMode()
{
	bUseSeamlessTravel = true;
}

UClass* ACGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	ACPlayerState* PlayerState = InController->GetPlayerState<ACPlayerState>();
	if (PlayerState)
	{
		return PlayerState->GetPickedCharacterClass().Get();
	}

	return Super::GetDefaultPawnClassForController(InController);
}
