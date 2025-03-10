// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Blueprint/UserWidget.h"
#include "UIWeaponSlot.generated.h"

/**
 * 
 */

// 무기 슬롯 UI 관련 정보를 저장하는 구조체 선언
USTRUCT(BlueprintType)
struct FWeaponSlotUI
{
	GENERATED_BODY()
	
	UPROPERTY()
	class UOverlay* GreenIndicator = nullptr; // 초록색 UI
	UPROPERTY()
	class UImage* WeaponImage = nullptr;   // 무기 아이콘 이미지
};

UCLASS()
class RIGHT4DEAD_API UUIWeaponSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateSlot(AWeaponBase* EquippedWeapon, const TMap<EWeaponType, AWeaponBase*>& Inventory);

	// Image
	UPROPERTY(meta = (BindWidget))
	class UImage* FirstBGImage;
	UPROPERTY(meta = (BindWidget))
	class UImage* SecondBGImage;
	UPROPERTY(meta = (BindWidget))
	class UImage* ThirdBGImage;
	UPROPERTY(meta = (BindWidget))
	class UImage* FourthBGImage;
	UPROPERTY(meta = (BindWidget))
	class UImage* FifthBGImage;

	// Equip Slot_OFF
	UPROPERTY(meta = (BindWidget))
	class UOverlay* First_OFF;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Second_OFF;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Third_OFF;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Fourth_OFF;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Fifth_OFF;
	
	// Equip Slot_ON
	UPROPERTY(meta = (BindWidget))
	class UOverlay* First_ON;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Second_ON;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Third_ON;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Fourth_ON;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Fifth_ON;

	// Ammo Count
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentAmmo;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxAmmo;

	// // Animation
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	// class UWidgetAnimation* UZI_Anim;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	// class UWidgetAnimation* Axe_Anim;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	// class UWidgetAnimation* Pipe_Anim;


	// 슬롯 UI 데이터 배열
	TArray<FWeaponSlotUI> WeaponSlots;

	void SetAmmoText();
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
