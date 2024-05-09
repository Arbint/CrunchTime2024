// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

UCLASS()
class ACharacterDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterDisplay();
	void SetCharacterWithDefination(const class UCharacterDefination* CharacterDef);

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Display")
	class USkeletalMeshComponent* DisplayMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Display")
	class UCameraComponent* ViewCam;
};
