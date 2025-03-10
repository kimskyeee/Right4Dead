// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"

#include "Right4DeadGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATank::ATank()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshObj(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/Zombie/Tank/Model/Tank.Tank'"));
	if (SkeletalMeshObj.Succeeded())
	{
		GetMesh()->SetSkeletalMeshAsset(SkeletalMeshObj.Object);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -89));
		GetMesh()->SetRelativeRotation(FRotator(0, 270, 0));
		ConstructorHelpers::FClassFinder<UAnimInstance> AnimBlueprintClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Zombies/Tank/ABP_Tank.ABP_Tank_C'"));
		if (AnimBlueprintClass.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(AnimBlueprintClass.Class);
		}
	}

	// SpecialZombieFSM = CreateDefaultSubobject<UTankFSM>(TEXT("SpecialZombieFSM"));
    // ZombieFSM = SpecialZombieFSM;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
}

void ATank::InitData()
{
	Speed = 210.0f;
	PartDamageMultipliers.Legs = 1;
	PartDamageMultipliers.Stomach = 1;
	PartDamageMultipliers.Thorax = 1;
	
	if (const URight4DeadGameInstance* GameInstance = Cast<URight4DeadGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		switch (GameInstance->GetDifficulty())
		{
		case EDifficulty::Easy:
			Hp = MaxHp = 3000.0f;
			break;
		case EDifficulty::Normal:
			Hp = MaxHp = 4000.0f;
			break;
		case EDifficulty::Advanced:
			Hp = MaxHp = 8000.0f;
			break;
		case EDifficulty::Expert:
			Hp = MaxHp = 8000.0f;
			break;
		}
	}
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}