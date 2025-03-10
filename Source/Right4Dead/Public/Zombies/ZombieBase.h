// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

class UZombieEffectComponent;
class UZombieAudioComponent;
class AZombieSpawnManager;
enum class EZombieState : uint8;
class UZombieAnimInstance;
class AAIController;
class UZombieBaseFSM;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletHit, const FHitResult&, HitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMeleeHit, const FVector&, BoneLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDismemberment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShove);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

USTRUCT()
struct FPartDamageMultipliers
{
	GENERATED_BODY()
	float Head = 4;
	float Stomach = 1.25f;
	float Legs = 0.75f;
	float Thorax = 1;
};

UCLASS(Abstract)
class RIGHT4DEAD_API AZombieBase : public ACharacter, public IActorBase
{
	GENERATED_BODY()

public:
	AZombieBase();

private:
	UFUNCTION(CallInEditor, Category="Debugging")
	void ForceDie();
	float FinalDamage;

public:
	UPROPERTY()
	TObjectPtr<AZombieSpawnManager> SpawnManager = nullptr;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UZombieBaseFSM> ZombieFSM = nullptr;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AAIController> AIController = nullptr;
	UPROPERTY()
	TObjectPtr<UZombieAnimInstance> ZombieAnimInstance = nullptr;
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UZombieAudioComponent> ZombieAudioComponentFactory = nullptr;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UZombieAudioComponent> ZombieAudioComponent = nullptr;

	float MaxHp;
	UPROPERTY(EditInstanceOnly, Category=Debugging)
	float Hp;
	float Speed;
	float TakeDamageMultiplier;
	FPartDamageMultipliers PartDamageMultipliers;
	bool bTakeDamaged;
	virtual void BeginPlay() override;
	virtual void InitStart();
	virtual void InitData() PURE_VIRTUAL(AZombieBase::InitData, )
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable)
	FOnBulletHit OnBulletHit;
	UPROPERTY(BlueprintAssignable)
	FOnMeleeHit OnMeleeHit;
	UPROPERTY(BlueprintAssignable)
	FOnDismemberment OnDismemberment;
	UPROPERTY(BlueprintAssignable)
	FOnShove OnShove;
	UPROPERTY(BlueprintAssignable)
	FOnDead OnDead;
	
	void HandleDieFromExplosion(const FVector& ExplosionOrigin, const float& Radius) const;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION()
	void OnTakeAnyDamageHandler(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION()
	void OnTakePointDamageHandler(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent,
								  FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	UFUNCTION()
	void OnTakeRadialDamageHandler(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo,
								   class AController* InstigatedBy, AActor* DamageCauser);
	virtual void HandleDie();

	float NormalAttackDamage = 1;
	virtual void HandleNormalAttack();
	virtual void HandleSpecialAttack();
	virtual void HandleShove(const FVector& FromLocation);
	void HandleStartChase(const TObjectPtr<AActor>& Target) const;
	void HandleStopChase() const;
	virtual void OnDamaged(float Damage) override;
	virtual void OnDie() override;

	FTransform ClimbDestination = FTransform::Identity;
	bool bClimbing = false;
	void StartClimbing(const FTransform& Destination);
	void EndClimbing();

	EZombieState GetState() const;
};