// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LobbyPlayerEntry.h"
#include "Framework/CPlayerState.h"
#include "Components/TextBlock.h"
void ULobbyPlayerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	ACPlayerState* PlayerState = Cast<ACPlayerState>(ListItemObject);
	if (PlayerState)
	{
		PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
	}
}
