// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	//game instance is created here, after 
	virtual void InitGameState() override;
};
