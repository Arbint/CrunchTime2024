// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LobbyGameMode.h"
#include "Framework/CGameState.h"
#include "Framework/CGameInstance.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::InitGameState()
{
	Super::InitGameState();
	ACGameState* CGameState = Cast<ACGameState>(GameState);
	UCGameInstance* GameInst = GetGameInstance<UCGameInstance>();

	if (CGameState && GameInst)
	{
		CGameState->SetSessionName(GameInst->GetCurrentSessionName());
	}
}
