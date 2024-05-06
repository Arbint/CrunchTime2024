// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/LobbyWidget.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "Framework/CGameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TileView.h"
#include "Framework/CharacterDefination.h"
#include "Framework/CPlayerState.h"
#include "Widgets/CharacterDefinationEntry.h"
#include "Components/Button.h"
#include "Framework/CGameInstance.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GameState = Cast<ACGameState>(UGameplayStatics::GetGameState(this));
	PlayerState = GetOwningPlayerState<ACPlayerState>();
	if(GameState)
	{ 
		FName LobbyName = GameState->GetSessonName();
		LobbyNameText->SetText(FText::FromName(LobbyName));
		GameState->OnSessionNameUpdated.AddUObject(this, &ULobbyWidget::SessionNameUpdated);

		PlayerList->SetListItems(GameState->PlayerArray);

		GetWorld()->GetTimerManager().SetTimer(PlayerListUpdateTimerHandle, this, &ULobbyWidget::RefreshPlayerList, 1, true);

		CharacterList->SetListItems(GameState->GetCharacterDefinations());
		CharacterList->OnItemSelectionChanged().AddUObject(this, &ULobbyWidget::PlayerSelectedIssued);
		GameState->OnCharacterSelectedReplicated.AddUObject(this, &ULobbyWidget::CharcterSelectionReplicated);
	}
	StartBtn->OnClicked.AddDynamic(this, &ULobbyWidget::LoadGame);
}

void ULobbyWidget::LoadGame()
{
	UCGameInstance* GameInst = GetGameInstance<UCGameInstance>();
	if(GameInst)
	{ 
		GameInst->LoadGameLevel();
	}
}

void ULobbyWidget::SessionNameUpdated(const FName& NewName)
{
	LobbyNameText->SetText(FText::FromName(NewName));
}

void ULobbyWidget::RefreshPlayerList()
{
	if (GameState)
	{
		PlayerList->SetListItems(GameState->PlayerArray);
	}
}

void ULobbyWidget::PlayerSelectedIssued(UObject* Item)
{
	UCharacterDefination* SelectedCharacterDefination = Cast<UCharacterDefination>(Item);
	if(SelectedCharacterDefination)
	{ 
		PlayerState->Server_IssueCharacterPick(SelectedCharacterDefination);
	}
}

void ULobbyWidget::CharcterSelectionReplicated(const UCharacterDefination* Selected, const UCharacterDefination* Deselected)
{
	if (Selected != nullptr)
	{
		UCharacterDefinationEntry* Entry = CharacterList->GetEntryWidgetFromItem<UCharacterDefinationEntry>(Selected);
		Entry->SetCharacterSelected(true);
	}

	if (Deselected)
	{
		UCharacterDefinationEntry* Entry = CharacterList->GetEntryWidgetFromItem<UCharacterDefinationEntry>(Deselected);
		Entry->SetCharacterSelected(false);
	}
}

