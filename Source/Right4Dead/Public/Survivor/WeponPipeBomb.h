// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Survivor.h"
#include "WeaponBase.h"
#include "Components/SpotLightComponent.h"
#include "WeponPipeBomb.generated.h"

UCLASS()
class RIGHT4DEAD_API AWeponPipeBomb : public AWeaponBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeponPipeBomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* TrailParticle;  
	
	UPROPERTY()
	ASurvivor* me;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsThrown = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bHasLanded = false;
	
	// 파이프 폭탄 관리 변수들
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float LIfeTime = 6.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentLifeTime = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentBeepTime = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float BeepInterval = 0.8f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentAggroTime = 0.0f;
	
	FTimerHandle PipeBombTraceTimerHandle;
	FTimerHandle BeepSoundTimerHandle;
	FTimerHandle ExplosionTimerHandle;

	UFUNCTION()
	void PipeBombInteraction();
	UFUNCTION()
	void ThrowWeapon();
	UFUNCTION()
	void OnThrowWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	                      UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	                      const FHitResult& Hit);
	UFUNCTION()
	void PipeBombTraceZombies();
	UFUNCTION()
	void ExplodeWeapon();

	UFUNCTION(CallInEditor,BlueprintImplementableEvent,Category="PipeBomb")
	void PlayBeepSound(float DeltaTime);

	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplosionEffect;
	UPROPERTY(EditAnywhere)
	UPointLightComponent* BeepLight;
	
	class USoundWave* PipeBombBeep; //폭탄삐삡
	class USoundWave* PipeBombEnd; //폭탄터짐
};


