// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/CCharacterBase.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "GameplayAbilities/CAbilitySystemComponent.h"
#include "GameplayAbilities/CAttributeSet.h"
#include "GameplayAbilities/CAbilityGenericTags.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Net/UnrealNetwork.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Touch.h"

#include "Targeting/TargetingBoxComponent.h"
#include "Widgets/StatusGuage.h"

// Sets default values
ACCharacterBase::ACCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("Attribute Set");

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true); // replicate means it is synced with the server.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &ACCharacterBase::HealthUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ACCharacterBase::MaxHealthUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &ACCharacterBase::ManaUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ACCharacterBase::MaxManaUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetExperienceAttribute()).AddUObject(this, &ACCharacterBase::ExperienceUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ACCharacterBase::NextLevelExperienceUpdated);
	AbilitySystemComponent->RegisterGameplayTagEvent(UCAbilityGenericTags::GetDeadTag()).AddUObject(this, &ACCharacterBase::DeathTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(UCAbilityGenericTags::GetAimingTag()).AddUObject(this, &ACCharacterBase::AimingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(UCAbilityGenericTags::GetStunedTag()).AddUObject(this, &ACCharacterBase::StunTagChanged);

	StatusWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Status Widget Comp");
	StatusWidgetComp->SetupAttachment(GetRootComponent());

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,  ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACCharacterBase::HitDetected);

	TargetingBoxComponent = CreateDefaultSubobject<UTargetingBoxComponent>("Targeting Box Component");
	TargetingBoxComponent->SetupAttachment(GetMesh());

	AIPerceptionSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("AI Perception Source Comp");
	AIPerceptionSourceComp->RegisterForSense(UAISense_Sight::StaticClass());
}

void ACCharacterBase::SetupAbilitySystemComponent()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ACCharacterBase::InitAttributes()
{
	AbilitySystemComponent->ApplyInitialEffects();
}

void ACCharacterBase::InitAbilities()
{
	AbilitySystemComponent->GrantInitialAbilities();
}

// Called when the game starts or when spawned
void ACCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	//LogAttributeSet();
	InitStatusHUD();
}

// Called every frame
void ACCharacterBase::Tick(float DeltaTime)
{
	//LogAttributeSet();
	Super::Tick(DeltaTime);
}

void ACCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//we are AI
	if (NewController && !NewController->IsPlayerController())
	{
		SetupAbilitySystemComponent();
		InitAttributes();
		InitAbilities();
	}

	if (HasAuthority() && Controller && Controller->IsPlayerController())
	{
		APlayerController* OwningPlayerController = Cast<APlayerController>(Controller);
		//TODO: Figure out the ID
		TeamId = FGenericTeamId(1);
	}
}

// Called to bind functionality to input
void ACCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACCharacterBase::PlayMontage(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay)
	{
		if (GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay);
		}
	}
}

float ACCharacterBase::GetAttributeValue(const FGameplayAttribute& Attr) const
{
	float val = 0;
	if (AbilitySystemComponent)
	{
		bool found = 0;
		val = AbilitySystemComponent->GetGameplayAttributeValue(Attr, found);
	}

	return val;
}

float ACCharacterBase::GetHealth() const
{
	return GetAttributeValue(UCAttributeSet::GetHealthAttribute());
}

float ACCharacterBase::GetMaxHealth() const
{
	return GetAttributeValue(UCAttributeSet::GetMaxHealthAttribute());
}

float ACCharacterBase::GetMana() const
{
	return GetAttributeValue(UCAttributeSet::GetManaAttribute());
}

float ACCharacterBase::GetMaxMana() const
{
	return GetAttributeValue(UCAttributeSet::GetMaxManaAttribute());
}

float ACCharacterBase::GetExperience() const
{
	return GetAttributeValue(UCAttributeSet::GetExperienceAttribute());
}

float ACCharacterBase::GetNextLevelExperience() const
{
	return GetAttributeValue(UCAttributeSet::GetNextLevelExperienceAttribute());
}

UAbilitySystemComponent* ACCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACCharacterBase::InitStatusHUD()
{
	StatusGuage = Cast<UStatusGuage>(StatusWidgetComp->GetUserWidgetObject());
	if (!StatusGuage)
	{
		UE_LOG(LogTemp, Error, TEXT("%s can't spawn status guage hud, status widget component has the wrong widget setup"), *GetName());


		return;
	}

	StatusGuage->SetRenderScale(FVector2D{0.5f});
	StatusGuage->SetHealth(GetHealth(), GetMaxHealth());
	StatusGuage->SetMana(GetMana(), GetMaxMana());

	if (IsLocallyControlled())
	{
		if(GetController() && GetController()->IsPlayerController())
			StatusGuage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ACCharacterBase::AimingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bIsAiming = NewStackCount != 0;
	AimingTagChanged(bIsAiming);
}

void ACCharacterBase::StunTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	if (NewStackCount != 0)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		AAIController* AIC = GetController<AAIController>();
		if (AIC)
		{
			AIC->GetBrainComponent()->StopLogic("Stunned");
		}
		TArray<FGameplayAbilitySpec> AllAbilities = GetAbilitySystemComponent()->GetActivatableAbilities();
		for (FGameplayAbilitySpec& Spec : AllAbilities)
		{
			GetAbilitySystemComponent()->CallServerEndAbility(Spec.Handle, Spec.ActivationInfo, FPredictionKey());
		}
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		AAIController* AIC = GetController<AAIController>();
		if (AIC)
		{
			AIC->GetBrainComponent()->StartLogic();
		}
	}
}

void ACCharacterBase::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if(StatusGuage)
		StatusGuage->SetHealth(ChangeData.NewValue, GetMaxHealth());

	if (HasAuthority())
	{
		if (ChangeData.NewValue >= GetMaxHealth())

		{
			AbilitySystemComponent->AddLooseGameplayTag(UCAbilityGenericTags::GetFullHealthTag());
		}
		else
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(UCAbilityGenericTags::GetFullHealthTag());
		}
	}

	if (ChangeData.NewValue <= 0)
	{
		StartDeath();
		if (HasAuthority() && ChangeData.GEModData)
		{
			UAbilitySystemComponent* KillerASC = ChangeData.GEModData->EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();
			if (KillerASC)
			{
				FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DeadRewardEffect, 1, GetAbilitySystemComponent()->MakeEffectContext());
				KillerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void ACCharacterBase::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (StatusGuage)
		StatusGuage->SetMana(ChangeData.NewValue, GetMaxMana());

	if (HasAuthority())
	{
		if (ChangeData.NewValue >= GetMaxMana())
		{
			AbilitySystemComponent->AddLooseGameplayTag(UCAbilityGenericTags::GetFullManaTag());
		}
		else
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(UCAbilityGenericTags::GetFullManaTag());
		}
	}
}
void ACCharacterBase::MaxHealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (StatusGuage)
		StatusGuage->SetHealth(GetHealth(), ChangeData.NewValue);
}

void ACCharacterBase::MaxManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (StatusGuage)
		StatusGuage->SetMana(GetMana(), ChangeData.NewValue);
}

void ACCharacterBase::ExperienceUpdated(const FOnAttributeChangeData& ChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT("Experience is now: %f"), ChangeData.NewValue);
	if (HasAuthority())
	{
		if (ChangeData.NewValue >= GetNextLevelExperience())
		{
			LevelUp();
		}
	}
}

void ACCharacterBase::NextLevelExperienceUpdated(const FOnAttributeChangeData& ChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT("Next Level Experience is now: %f"), ChangeData.NewValue);
	if (HasAuthority())
	{
		if (ChangeData.NewValue <= GetExperience())
		{
			LevelUp();
		}
	}
}

void ACCharacterBase::LevelUp()
{
		FGameplayEffectSpecHandle Spec = GetAbilitySystemComponent()->MakeOutgoingSpec(LevelUpEffect, 1, GetAbilitySystemComponent()->MakeEffectContext());
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}

void ACCharacterBase::StartStunAnim()
{
	PlayMontage(StunMontage);
}

void ACCharacterBase::StopStunAnim()
{
	GetMesh()->GetAnimInstance()->Montage_Stop(0.2, StunMontage);
}

void ACCharacterBase::PlayHitReaction()
{
	PlayMontage(HitReactionMontage);
}

void ACCharacterBase::StartDeath()
{
	PlayMontage(DeathMontage);
	AbilitySystemComponent->ApplyGameplayEffect(DeathEffect);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AIPerceptionSourceComp->UnregisterFromPerceptionSystem();
	OnDeadStatusChanged.Broadcast(true);
}

void ACCharacterBase::DeathTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	if (NewStackCount == 0)
	{
		StopAnimMontage(DeathMontage);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		AbilitySystemComponent->ApplyFullStat();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AIPerceptionSourceComp->RegisterWithPerceptionSystem();
		OnDeadStatusChanged.Broadcast(false);
	}
}

void ACCharacterBase::ClientPlayAnimMontage_Implementation(UAnimMontage* MontageToPlay)
{
	if(!HasAuthority())
		PlayAnimMontage(MontageToPlay);
}

void ACCharacterBase::ClientStopAnimMontage_Implementation(UAnimMontage* MontageToStop)
{
	if(!HasAuthority())
		StopAnimMontage(MontageToStop);
}

void ACCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACCharacterBase, TeamId, COND_None);
}

void ACCharacterBase::HitDetected(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UAISense_Touch::ReportTouchEvent(this, OtherActor, this, GetActorLocation());
}

void ACCharacterBase::LogAttributeSet() const
{
	if (CAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute Set Exists"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute Set Missing!!!"));
	}
}

