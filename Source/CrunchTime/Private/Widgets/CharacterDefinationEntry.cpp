// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterDefinationEntry.h"
#include "Framework/CharacterDefination.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCharacterDefinationEntry::SetCharacterSelected(bool bIsCharacterSelected)
{
	SetIsEnabled(!bIsCharacterSelected);
	IconImage->GetDynamicMaterial()->SetScalarParameterValue(SatrurationParamName, bIsCharacterSelected ? 0 : 1);
}

void UCharacterDefinationEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	CharacterDef = Cast<UCharacterDefination>(ListItemObject);

	if (CharacterDef)
	{
		NameText->SetText(FText::FromName(CharacterDef->GetCharacterName()));
		IconImage->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, CharacterDef->GetIcon());
	}
}
