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
#include "Player/CharacterDisplay.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GameState = Cast<ACGameState>(UGameplayStatics::GetGameState(this));
	PlayerState = GetOwningPlayerState<ACPlayerState>();
	if (PlayerState)
	{
		PlayerState->OnPickedCharacterReplicated.AddUObject(this, &ULobbyWidget::UpdateCharacterDisplay);
	}

	if(GameState)
	{ 
		FName LobbyName = GameState->GetSessonName();
		LobbyNameText->SetText(FText::FromName(LobbyName));
		GameState->OnSessionNameUpdated.AddUObject(this, &ULobbyWidget::SessionNameUpdated);

		PlayerList->SetListItems(GameState->PlayerArray);

		GetWorld()->GetTimerManager().SetTimer(RefreshLobbyStatusTimerHandle, this, &ULobbyWidget::RefreshLobbyStats, 1, true);

		CharacterList->SetListItems(GameState->GetCharacterDefinations());
		CharacterList->OnItemSelectionChanged().AddUObject(this, &ULobbyWidget::PlayerSelectedIssued);
		CharacterList->OnEntryWidgetGenerated().AddUObject(this, &ULobbyWidget::CharacterDefinationWidgetCreated);

		GameState->OnCharacterSelectionUpdated.AddUObject(this, &ULobbyWidget::CharcterSelectionReplicated);
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

void ULobbyWidget::SpawnCharacterDisplay()
{
	if (CharcterDisplayClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning Charcter Display!"))
		FActorSpawnParameters SpawnParam;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CharacterDisplay = GetWorld()->SpawnActor<ACharacterDisplay>(CharcterDisplayClass, SpawnParam);
		GetOwningPlayer()->SetViewTarget(CharacterDisplay);
	}
}

void ULobbyWidget::UpdateCharacterDisplay(const UCharacterDefination* NewDefination)
{
	if (!CharacterDisplay)
	{
		SpawnCharacterDisplay();
	}

	if (CharacterDisplay)
	{
		CharacterDisplay->SetCharacterWithDefination(NewDefination);
	}
}


void ULobbyWidget::SessionNameUpdated(const FName& NewName)
{
	LobbyNameText->SetText(FText::FromName(NewName));
}

void ULobbyWidget::RefreshLobbyStats()
{
	if (!GameState)
	{
		return;
	}
	
	PlayerList->SetListItems(GameState->PlayerArray);
}

void ULobbyWidget::PlayerSelectedIssued(UObject* Item)
{
	UCharacterDefination* SelectedCharacterDefination = Cast<UCharacterDefination>(Item);
	if(SelectedCharacterDefination)
	{ 
		PlayerState->Server_IssueCharacterPick(SelectedCharacterDefination);
	}
}

void ULobbyWidget::CharcterSelectionReplicated(const TArray<const UCharacterDefination*>& SelectedCharacters)
{
	for (UUserWidget* Entry : CharacterList->GetDisplayedEntryWidgets())
	{
		UCharacterDefinationEntry* DefinationEntry = Cast<UCharacterDefinationEntry>(Entry);
		if (DefinationEntry)
		{
			DefinationEntry->SetCharacterSelected(false);
			if (SelectedCharacters.Contains(DefinationEntry->GetCharacterDefination()))
			{
				DefinationEntry->SetCharacterSelected(true);
			}
		}
	}
}

void ULobbyWidget::CharacterDefinationWidgetCreated(UUserWidget& NewEntry)
{
	UCharacterDefinationEntry* DefinationEntry = Cast<UCharacterDefinationEntry>(&NewEntry);
	const TArray<const UCharacterDefination*> SelectedCharacters = GameState->GetSelectedCharacters();
	if (DefinationEntry && SelectedCharacters.Contains(DefinationEntry->GetCharacterDefination()))
	{
		DefinationEntry->SetCharacterSelected(true);
	}
}
