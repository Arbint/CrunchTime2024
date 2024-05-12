// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacterBase.h"
#include "CPlayerCharacter.generated.h"

//forward declartion. I am telling the compiler that this class exists.
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class ACPlayerCharacter : public ACCharacterBase
{
	GENERATED_BODY()
public:
	//this is the construtor
	ACPlayerCharacter();

private:
	UPROPERTY(visibleAnywhere, Category="View")
	USpringArmComponent* cameraBoom;
	
	UPROPERTY(visibleAnywhere, Category="View")
	UCameraComponent* viewCamera;

	UPROPERTY(EditDefaultsOnly, Category="View")
	FVector AimCameraLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category="View")
	float AimCameraLerpingSpeed = 5;

	virtual void PawnClientRestart() override;

	/*****************************************************/
	/*                       Input                       */
	/*****************************************************/
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* inputMapping;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* moveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* lookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* jumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* baiscAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AbilityOneInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AbilityConfirmAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AbilityCancelAction;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& InputValue);

	UFUNCTION()
	void Look(const FInputActionValue& InputValue);

	UFUNCTION()
	void DoBasicAttack();
	
	UFUNCTION()
	void TryActivateAbilityOne();

	UFUNCTION()
	void ComfirmActionTriggered();
	
	UFUNCTION()
	void CancelActionTriggered();

	FVector GetMoveFwdDir() const;
	FVector GetMoveRightDir() const;

	virtual void AimingTagChanged(bool bNewIsAiming) override;
	
	void LerpCameraToLocalOffset(const FVector& LocalOffset);
	void TickCameraLocalOffset(FVector Goal);
	FTimerHandle CameraLerpHandle;
};
