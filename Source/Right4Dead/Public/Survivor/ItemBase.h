// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UCLASS()
class RIGHT4DEAD_API AItemBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//트레이싱 됐을때 overlay 재질 설정해주는 함수
	TArray<UMaterialInterface*> OriginalMaterials;
	virtual void SetOverlayMaterial(UMaterialInterface* MyOverlayMaterial);
	virtual void ClearOverlayMaterial();
	
	virtual void Interaction();
};
