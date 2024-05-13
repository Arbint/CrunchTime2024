// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CGameState.generated.h"

class UCharacterDefination;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionNameUpdated, const FName&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharcterSelectionUpdated, const TArray<const UCharacterDefination*>&/*Selected*/);

/**
 * 
 */
UCLASS()
class ACGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ACGameState();
	FOnSessionNameUpdated OnSessionNameUpdated;
	FOnCharcterSelectionUpdated OnCharacterSelectionUpdated;
	FName GetSessonName() const {return SessionName;}
	void SetSessionName(const FName& SessionName);
	void UpdateCharacterSelection(const UCharacterDefination* NewSelcted, const UCharacterDefination* DeSelected);
	const TArray<class UCharacterDefination*>& GetCharacterDefinations() const { return Characters; }
	bool IsCharacterSelected(const UCharacterDefination* CharacterToCheck) const;
	const TArray<const UCharacterDefination*>& GetSelectedCharacters() const;

private:
	UPROPERTY(ReplicatedUsing = OnRep_SessionName)
	FName SessionName;

	UFUNCTION()
	void OnRep_SessionName();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Main")
	TArray<UCharacterDefination*> Characters;

	UPROPERTY(ReplicatedUsing = OnRep_SelectedCharacters)
	TArray<const UCharacterDefination*> SelectedCharacters;

	UFUNCTION()
	void OnRep_SelectedCharacters();
};



