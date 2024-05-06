// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CGameState.generated.h"

class UCharacterDefination;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionNameUpdated, const FName&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterSelectionReplicated, const UCharacterDefination* /*Selected*/, const UCharacterDefination* /*Deslected*/);

/**
 * 
 */
UCLASS()
class ACGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	FOnSessionNameUpdated OnSessionNameUpdated;
	FOnCharacterSelectionReplicated OnCharacterSelectedReplicated;
	FName GetSessonName() const {return SessionName;}
	void SetSessionName(const FName& SessionName);
	void UpdateCharacterSelection(const UCharacterDefination* NewSelcted, const UCharacterDefination* DeSelected);
	const TArray<class UCharacterDefination*>& GetCharacterDefinations() const { return Characters; }
	bool IsCharacterSelected(const UCharacterDefination* CharacterToCheck) const;

private:
	UPROPERTY(ReplicatedUsing = OnRep_SessionName)
	FName SessionName;

	UFUNCTION()
	void OnRep_SessionName();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_UpdatedCharacterSelection(const UCharacterDefination* NewSelcted, const UCharacterDefination* DeSelected);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Main")
	TArray<UCharacterDefination*> Characters;

	UPROPERTY(EditDefaultsOnly, Category = "Main")
	TArray<const UCharacterDefination*> SelectedCharacters;
};

