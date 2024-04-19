// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CMainMenu.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Widgets/SessionEntry.h"
#include "Framework/CGameInstance.h"

void UCMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	LoginBtn->OnClicked.AddDynamic(this, &UCMainMenu::LoginBtnClicked);
	CreateSessionBtn->OnClicked.AddDynamic(this, &UCMainMenu::CreateSessionBtnClicked);
	CreateSessionBtn->SetIsEnabled(false);
	SessionNameText->OnTextChanged.AddDynamic(this, &UCMainMenu::SessionNameTextChanged);
	FindSessionBtn->OnClicked.AddDynamic(this, &UCMainMenu::FindSessionBtnClicked);

	CGameInst=GetGameInstance<UCGameInstance>();
	CGameInst->OnSessionSearchCompleted.AddUObject(this, &UCMainMenu::SessionSearchCompleted);
}

void UCMainMenu::SessionNameTextChanged(const FText& NewText)
{
	CreateSessionBtn->SetIsEnabled(!NewText.IsEmpty());
}

void UCMainMenu::LoginBtnClicked()
{
	if (CGameInst)
	{
		CGameInst->Login();
	}
}

void UCMainMenu::CreateSessionBtnClicked()
{
	if (CGameInst)
	{
		CGameInst->CreateSession(FName{SessionNameText->GetText().ToString()});
	}
}

void UCMainMenu::FindSessionBtnClicked()
{
	if (CGameInst)
	{
		CGameInst->FindSessions();
	}
}

void UCMainMenu::LoadSessionWithIndex(int Index)
{
	if (CGameInst)
	{
		CGameInst->JoinSessionWithSearchResultIndex(Index);
	}
}

void UCMainMenu::SessionSearchCompleted(const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Serach Completed And"));
	int index = 0;
	for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
	{
		FString SessionName;
		SearchResult.Session.SessionSettings.Get(CGameInst->GetSessionNameKey(), SessionName);
		USessionEntry* NewSessionEntry = CreateWidget<USessionEntry>(GetOwningPlayer(), SessionEntryClass);
		if (NewSessionEntry)
		{
			NewSessionEntry->SetSessionInfo(SessionName, index);
			NewSessionEntry->OnSessionBtnClicked.AddUObject(this, &UCMainMenu::LoadSessionWithIndex);
			SessionEntryList->AddChild(NewSessionEntry);
		}
		++index;
	}
}
