// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerCharacter.h"
#include "Player/useless.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayAbilities/CAbilitySystemTypes.h"

ACPlayerCharacter::ACPlayerCharacter()
{
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	viewCamera = CreateDefaultSubobject<UCameraComponent>("View Camera");
	
	cameraBoom->SetupAttachment(GetRootComponent());
	viewCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);

	cameraBoom->bUsePawnControlRotation = true;
	cameraBoom->TargetArmLength = 800.f;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(1080.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
}

void ACPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(inputMapping, 0);
	}
}

void ACPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PrintMsg();
	UEnhancedInputComponent* enhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComp)
	{
		enhancedInputComp->BindAction(moveInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Move);
		enhancedInputComp->BindAction(lookInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Look);
		enhancedInputComp->BindAction(jumpInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Jump);
		enhancedInputComp->BindAction(baiscAttackAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::DoBasicAttack);
		enhancedInputComp->BindAction(AbilityOneInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::TryActivateAbilityOne);
		enhancedInputComp->BindAction(AbilityConfirmAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::ComfirmActionTriggered);
		enhancedInputComp->BindAction(AbilityCancelAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::CancelActionTriggered);
	}
}

void ACPlayerCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D input = InputValue.Get<FVector2D>();
	input.Normalize();

	AddMovementInput(input.Y * GetMoveFwdDir() + input.X * GetMoveRightDir());
}

void ACPlayerCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D input = InputValue.Get<FVector2D>();
	AddControllerYawInput(input.X);
	AddControllerPitchInput(-input.Y);
}

void ACPlayerCharacter::DoBasicAttack()
{
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::BasicAttack);
}

void ACPlayerCharacter::TryActivateAbilityOne()
{
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::AbilityOne);
}

void ACPlayerCharacter::ComfirmActionTriggered()
{
	UE_LOG(LogTemp, Warning,TEXT("Confirmed"));
	GetAbilitySystemComponent()->InputConfirm();
}

void ACPlayerCharacter::CancelActionTriggered()
{
	UE_LOG(LogTemp, Warning,TEXT("Cancelled"));
	GetAbilitySystemComponent()->InputCancel();
}

FVector ACPlayerCharacter::GetMoveFwdDir() const
{
	FVector CamerFwd = viewCamera->GetForwardVector();
	CamerFwd.Z = 0;
	return CamerFwd.GetSafeNormal();
}

FVector ACPlayerCharacter::GetMoveRightDir() const
{
	return viewCamera->GetRightVector();
}

void ACPlayerCharacter::AimingTagChanged(bool bNewIsAiming)
{
	bUseControllerRotationYaw = bNewIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bNewIsAiming;
	if (bNewIsAiming)
	{
		LerpCameraToLocalOffset(AimCameraLocalOffset);
	}
	else
	{
		LerpCameraToLocalOffset(FVector::ZeroVector);
	}
}

void ACPlayerCharacter::LerpCameraToLocalOffset(const FVector& LocalOffset)
{
	GetWorldTimerManager().ClearTimer(CameraLerpHandle);
	CameraLerpHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ACPlayerCharacter::TickCameraLocalOffset, LocalOffset));
}

void ACPlayerCharacter::TickCameraLocalOffset(FVector Goal)
{
	FVector CurrentLocalOffset = viewCamera->GetRelativeLocation();
	if (FVector::Dist(CurrentLocalOffset, Goal) < 1)
	{
		viewCamera->SetRelativeLocation(Goal);
		return;
	}

	FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, Goal, GetWorld()->GetDeltaSeconds() * AimCameraLerpingSpeed);
	viewCamera->SetRelativeLocation(NewLocalOffset);
	CameraLerpHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ACPlayerCharacter::TickCameraLocalOffset, Goal));
}
