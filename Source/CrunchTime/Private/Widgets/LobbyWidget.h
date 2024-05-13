// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UCharacterDefination;
/**
 * 
 */
UCLASS()
class ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
private:
	UFUNCTION()
	void LoadGame();
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LobbyNameText;

	UPROPERTY(meta = (BindWidget))
	class UListView* PlayerList;

	UPROPERTY(meta = (BindWidget))
	class UTileView* CharacterList;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* StartBtn;

	UPROPERTY(EditDefaultsOnly, Category = "Display")
	TSubclassOf<class ACharacterDisplay> CharcterDisplayClass;

	UPROPERTY()
	ACharacterDisplay* CharacterDisplay;

	void SpawnCharacterDisplay();
	void UpdateCharacterDisplay(const class UCharacterDefination* NewDefination);

	UPROPERTY()
	class ACGameState* GameState;

	void SessionNameUpdated(const FName& NewName);

	FTimerHandle RefreshLobbyStatusTimerHandle;

	void RefreshLobbyStats();
	
	void PlayerSelectedIssued(UObject* Item);

	UPROPERTY()
	class ACPlayerState* PlayerState;

	void CharcterSelectionReplicated(const TArray<const UCharacterDefination*>& SelectedCharacters);

	void CharacterDefinationWidgetCreated(UUserWidget& NewEntry);
};


