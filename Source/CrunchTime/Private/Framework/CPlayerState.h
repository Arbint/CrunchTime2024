// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CPlayerState.generated.h"

class UCharacterDefination;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPickedCharacterReplicated, const UCharacterDefination*);
/**
 * 
 */
UCLASS()
class ACPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ACPlayerState();
	FOnPickedCharacterReplicated OnPickedCharacterReplicated;
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_IssueCharacterPick(class UCharacterDefination* NewPickedCharacterDefination);
private:
	UPROPERTY(ReplicatedUsing = OnRep_PickedCharacter)
	class UCharacterDefination* PickedCharacter;

	UFUNCTION()
	void OnRep_PickedCharacter();
protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};

