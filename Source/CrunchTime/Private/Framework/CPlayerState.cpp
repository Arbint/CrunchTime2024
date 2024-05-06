// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Framework/CGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/CharacterDefination.h"
ACPlayerState::ACPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.f;
}

const TSubclassOf<class ACharacter> ACPlayerState::GetPickedCharacterClass() const
{
	if (PickedCharacter)
	{
		return PickedCharacter->GetCharacterClass();
	}
	return TSubclassOf<ACharacter>();
}

void ACPlayerState::Server_IssueCharacterPick_Implementation(UCharacterDefination* NewPickedCharacterDefination)
{
	if (NewPickedCharacterDefination == PickedCharacter)
	{
		return;
	}
	
	ACGameState* GameState = Cast<ACGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState || GameState->IsCharacterSelected(NewPickedCharacterDefination))
	{
		return;
	}

	GameState->UpdateCharacterSelection(NewPickedCharacterDefination, PickedCharacter);
	PickedCharacter = NewPickedCharacterDefination;
	OnPickedCharacterReplicated.Broadcast(PickedCharacter);
}

bool ACPlayerState::Server_IssueCharacterPick_Validate(UCharacterDefination* NewPickedCharacterDefination)
{
	return true;
}

void ACPlayerState::OnRep_PickedCharacter()
{
	OnPickedCharacterReplicated.Broadcast(PickedCharacter);
}

void ACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ACPlayerState, PickedCharacter, COND_None, REPNOTIFY_Always);
}

void ACPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	ACPlayerState* CPlayerState = Cast<ACPlayerState>(PlayerState);
	if (CPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Transitioning to new player state"));
		CPlayerState->PickedCharacter = PickedCharacter;
	}
}
