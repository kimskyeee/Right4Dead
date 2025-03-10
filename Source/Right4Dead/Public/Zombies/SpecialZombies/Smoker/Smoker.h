// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialZombieBase.h"
#include "GameFramework/Actor.h"
#include "Smoker.generated.h"

UCLASS()
class RIGHT4DEAD_API ASmoker : public ASpecialZombieBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmoker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitData() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
