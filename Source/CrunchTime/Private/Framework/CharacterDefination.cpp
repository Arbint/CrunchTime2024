// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CharacterDefination.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

USkeletalMesh* UCharacterDefination::GetMesh() const
{
	if (CharacterClass)
	{
		ACharacter* CharacterCDO = Cast<ACharacter>(CharacterClass->GetDefaultObject());
		if (CharacterCDO)
		{
			return CharacterCDO->GetMesh()->GetSkeletalMeshAsset();
		}
	}
	return nullptr;
}
