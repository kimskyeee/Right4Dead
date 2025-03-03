// Fill out your copyright notice in the Description page of Project Settings.


#include "Witch.h"

#include "Right4DeadGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWitch::AWitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpecialZombieFSM = CreateDefaultSubobject<UWitchFSM>(TEXT("SpecialZombieFSM"));
	// ZombieFSM = SpecialZombieFSM;
}

// Called when the game starts or when spawned
void AWitch::BeginPlay()
{
	Super::BeginPlay();
}

void AWitch::InitData()
{
	Hp = 1000.0f;
	Speed = 300.0f;
	PartDamageMultipliers.Legs = 1;
	PartDamageMultipliers.Stomach = 1;
	PartDamageMultipliers.Thorax = 1;
	
	if (const URight4DeadGameInstance* GameInstance = Cast<URight4DeadGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		switch (GameInstance->GetDifficulty())
		{
		case EDifficulty::Easy:
			break;
		case EDifficulty::Normal:
			break;
		case EDifficulty::Advanced:
			break;
		case EDifficulty::Expert:
			break;
		}
	}
}

// Called every frame
void AWitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}