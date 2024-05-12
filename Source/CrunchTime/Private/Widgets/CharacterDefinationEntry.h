// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterDefinationEntry.generated.h"

/**
 * 
 */
UCLASS()
class UCharacterDefinationEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	void SetCharacterSelected(bool bIsCharacterSelected);
	const class UCharacterDefination* GetCharacterDefination() const { return CharacterDef; }
protected:	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
private:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText;

	UPROPERTY()
	class UCharacterDefination* CharacterDef;

	UPROPERTY(EditDefaultsOnly)
	FName IconMaterialParamName {"Icon"};

	UPROPERTY(EditDefaultsOnly)
	FName SatrurationParamName {"Saturation"};
};
