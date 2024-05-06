// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDefination.generated.h"

/**
 * 
 */
UCLASS()
class UCharacterDefination : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	FName GetCharacterName() const { return CharacterName; }
	UTexture2D* GetIcon() const { return Icon; }
	TSubclassOf<class ACharacter> GetCharacterClass() const { return CharacterClass; }
	USkeletalMesh* GetMesh() const;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UAnimInstance> AnimBlueprint;

	UPROPERTY(EditDefaultsOnly)
	FName CharacterName;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACharacter> CharacterClass;

};
