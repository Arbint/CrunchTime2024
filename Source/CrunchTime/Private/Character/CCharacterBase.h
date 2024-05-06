// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"

#include "GameFramework/Character.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilities/CGameplayCueInterface.h"
#include "GenericTeamAgentInterface.h"

#include "CCharacterBase.generated.h"

class UCAbilitySystemComponent;
class UCAttributeSet;
class UGameplayEffect;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeadStatusChanged, bool/*isDead*/);

UCLASS()
class ACCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICGameplayCueInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	FOnDeadStatusChanged OnDeadStatusChanged;
	// Sets default values for this character's properties
	ACCharacterBase();
	void SetupAbilitySystemComponent();
	void InitAttributes();
	void InitAbilities();
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId;  }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//called on the server only, when posses by a controller.
	virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	class UTargetingBoxComponent* TargetingBoxComponent;

	void PlayMontage(UAnimMontage* MontageToPlay);

	/*************************************************************/
	/*                              Gameplay Ability                                  */
	/*************************************************************/
public:
	float GetAttributeValue(const FGameplayAttribute& Attr) const;
	float GetHealth() const;
	float GetMaxHealth() const;
	float GetMana() const;
	float GetMaxMana() const;
	float GetExperience() const;
	float GetNextLevelExperience() const;
	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void InitStatusHUD();
private:
	void AimingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void AimingTagChanged(bool bNewIsAiming) {/*empty in base*/};
	bool bIsAiming;

	void StunTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);

	UPROPERTY(VisibleAnywhere, Category = "Gameplay Ability")
	UCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UCAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* StatusWidgetComp;

	UPROPERTY()
	class UStatusGuage* StatusGuage;

	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void MaxHealthUpdated(const FOnAttributeChangeData& ChangeData);

	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	void MaxManaUpdated(const FOnAttributeChangeData& ChangeData);

	void ExperienceUpdated(const FOnAttributeChangeData& ChangeData);
	void NextLevelExperienceUpdated(const FOnAttributeChangeData& ChangeData);
	void LevelUp();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TSubclassOf<UGameplayEffect> DeadRewardEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TSubclassOf<UGameplayEffect> LevelUpEffect;

	/*************************************************************/
	/*                              Gameplay Cue                                   */
	/*************************************************************/
public:
	virtual void StartStunAnim() override;
	virtual void StopStunAnim() override;
	virtual void PlayHitReaction() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Cue")
	UAnimMontage* HitReactionMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Cue")
	UAnimMontage* StunMontage;

	/*************************************************************/
	/*                                      Death                                      */
	/*************************************************************/
private:
	void StartDeath();
	
	void DeathTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TSubclassOf<UGameplayEffect> DeathEffect;
	
	/*************************************************************/
	/*                                          AI                                           */
	/*************************************************************/
public:
	UFUNCTION(NetMulticast, unreliable)
	void ClientPlayAnimMontage(UAnimMontage* MontageToPlay);

	UFUNCTION(NetMulticast, unreliable)
	void ClientStopAnimMontage(UAnimMontage* MontageToStop);

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(VisibleDefaultsOnly, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* AIPerceptionSourceComp;

	UFUNCTION()
	void HitDetected(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
