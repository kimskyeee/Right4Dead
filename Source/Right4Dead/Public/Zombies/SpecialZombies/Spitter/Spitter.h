// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialZombieBase.h"
#include "GameFramework/Actor.h"
#include "Spitter.generated.h"

UCLASS()
class RIGHT4DEAD_API ASpitter : public ASpecialZombieBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpitter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitData() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
