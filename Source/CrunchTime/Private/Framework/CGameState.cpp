// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameState.h"
#include "Net/UnrealNetwork.h"
#include "Framework/CharacterDefination.h"

void ACGameState::SetSessionName(const FName& NewSessionName)
{
	SessionName = NewSessionName;
}

void ACGameState::UpdateCharacterSelection(const UCharacterDefination* NewSelcted, const UCharacterDefination* DeSelected)
{
	if (!HasAuthority()) return;

	if (NewSelcted != nullptr)
	{
		SelectedCharacters.Add(NewSelcted);
	}

	if (DeSelected)
	{
		SelectedCharacters.Remove(DeSelected);
	}

	NetMulticast_UpdatedCharacterSelection(NewSelcted, DeSelected);
}

bool ACGameState::IsCharacterSelected(const UCharacterDefination* CharacterToCheck) const
{
	return SelectedCharacters.Contains(CharacterToCheck);
}

void ACGameState::OnRep_SessionName()
{
	OnSessionNameUpdated.Broadcast(SessionName);
}

void ACGameState::NetMulticast_UpdatedCharacterSelection_Implementation(const UCharacterDefination* NewSelcted, const UCharacterDefination* DeSelected)
{
	OnCharacterSelectedReplicated.Broadcast(NewSelcted, DeSelected);
}

void ACGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ACGameState, SessionName, COND_None, REPNOTIFY_Always);
}
