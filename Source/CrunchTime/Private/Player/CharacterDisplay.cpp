// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/CharacterDisplay.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Framework/CharacterDefination.h"
#include "Camera/CameraComponent.h"
// Sets default values
ACharacterDisplay::ACharacterDisplay()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(false);
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComp");
	DisplayMesh = CreateDefaultSubobject<USkeletalMeshComponent>("DisplayMesh");
	DisplayMesh->SetupAttachment(GetRootComponent());
	ViewCam = CreateDefaultSubobject<UCameraComponent>("ViewCam");
	ViewCam->SetupAttachment(GetRootComponent());
}

void ACharacterDisplay::SetCharacterWithDefination(const UCharacterDefination* CharacterDef)
{
	if (CharacterDef && DisplayMesh)
	{
		DisplayMesh->SetSkeletalMesh(CharacterDef->GetMesh());
		DisplayMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		DisplayMesh->SetAnimInstanceClass(CharacterDef->GetAnimInstance());
	}
}

