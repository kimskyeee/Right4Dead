// Fill out your copyright notice in the Description page of Project Settings.


#include "Survivor.h"

#include "CommonZombie.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EWeaponType.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "ItemBase.h"
#include "SafeDoor.h"
#include "ShoveDamageType.h"
#include "SurvivorArmAnim.h"
#include "UIAttackZombie.h"
#include "UISurvivorCrosshair.h"
#include "UISurvivorMain.h"
#include "UISurvivorMedKit.h"

#include "UITakeDamage.h"
#include "UIWeaponSlot.h"
#include "WeaponBase.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "BehaviorTree/Tasks/BTTask_PlayAnimation.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/LogTrace.h"
#include "Right4Dead/Right4Dead.h"
#include "Tests/AutomationCommon.h"

// Sets default values
ASurvivor::ASurvivor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Head=CreateDefaultSubobject<USkeletalMeshComponent>("Head");
	Head->SetupAttachment(GetMesh());

	//기본 카메라 설정
	FirstCameraComp=CreateDefaultSubobject<UCameraComponent>(TEXT("FirstCameraComp"));
	FirstCameraComp->SetupAttachment(Head);
	FirstCameraComp->SetRelativeLocationAndRotation(FVector(0,0,160),FRotator(0,90,0));
	FirstCameraComp->bUsePawnControlRotation = true;
	
	ThirdPerson=CreateDefaultSubobject<USkeletalMeshComponent>("ThirdPerson");
	ThirdPerson->SetupAttachment(RootComponent);
	ThirdPerson->SetVisibility(false);
	
	SpringArmComp=CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(ThirdPerson);
	SpringArmComp->SetRelativeLocation(FVector(0,0,70));
	SpringArmComp->TargetArmLength = 300.f;
	SpringArmComp->bUsePawnControlRotation = true;
	
	ThirdPersonCameraComp=CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdCameraComp"));
	ThirdPersonCameraComp->SetupAttachment(SpringArmComp);

	Arms=CreateDefaultSubobject<USkeletalMeshComponent>("Arms");
	Arms->SetupAttachment(FirstCameraComp);
	
	bFirstPerson = true;

	//몽타주 연동
	ConstructorHelpers::FObjectFinder<UAnimMontage> TempShoveMontage(TEXT("/Script/Engine.AnimMontage'/Game/UltimateFPSAnimationsKIT/Animations/Arms/swingmontage_new.swingmontage_new'"));
	if (TempShoveMontage.Succeeded())
	{
		ShoveMontage = TempShoveMontage.Object;
	}

	//Input데이터 할당하기
	//이동관련
	ConstructorHelpers::FObjectFinder<UInputMappingContext> TempIMC(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Survivor.IMC_Survivor'"));
	if (TempIMC.Succeeded())
	{
		IMC_Survivor=TempIMC.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIAMove(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurMove.IA_SurMove'"));
	if (TempIAMove.Succeeded())
	{
		IA_SurMove=TempIAMove.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIATurn(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurTurn.IA_SurTurn'"));
	if (TempIATurn.Succeeded())
	{
		IA_SurTurn=TempIATurn.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIALook(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurLook.IA_SurLook'"));
	if (TempIALook.Succeeded())
	{
		IA_SurLook=TempIALook.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIAJump(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurJump.IA_SurJump'"));
	if (TempIALook.Succeeded())
	{
		IA_SurJump=TempIAJump.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIACrouch(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurCrouch.IA_SurCrouch'"));
	if (TempIACrouch.Succeeded())
	{
		IA_SurCrouch=TempIACrouch.Object;
	}

	//공격관련 input
	ConstructorHelpers::FObjectFinder<UInputAction> TempIAFire(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurFire.IA_SurFire'"));
	if (TempIAFire.Succeeded())
	{
		IA_SurFire=TempIAFire.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIARelaod(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurReload.IA_SurReload'"));
	if (TempIARelaod.Succeeded())
	{
		IA_SurReload=TempIARelaod.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIARight(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurRight.IA_SurRight'"));
	if (TempIARight.Succeeded())
	{
		IA_SurRight=TempIARight.Object;
	}

	//무기전환 input
	ConstructorHelpers::FObjectFinder<UInputAction> TempIAPrimary(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Weapon1.IA_Weapon1'"));
	if (TempIAPrimary.Succeeded())
	{
		IA_PrimaryWeapon=TempIAPrimary.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIASecondary(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Weapon2.IA_Weapon2'"));
	if (TempIASecondary.Succeeded())
	{
		IA_SecondaryWeapon=TempIASecondary.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIAMelee(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Weapon3.IA_Weapon3'"));
	if (TempIAMelee.Succeeded())
	{
		IA_MeleeWeapon=TempIAMelee.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> TempIAHandle(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Weapon4.IA_Weapon4'"));
	if (TempIAHandle.Succeeded())
	{
		IA_HandleObject=TempIAHandle.Object;
	}

	//무기줍기
	ConstructorHelpers::FObjectFinder<UInputAction> TempIAPickUp(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_SurPickUp.IA_SurPickUp'"));
	if (TempIAPickUp.Succeeded())
	{
		IA_PickUp=TempIAPickUp.Object;
	}

	//overlap되면 Material Instance (Overlay 설정)
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> TempWeaponOverlay(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Blueprints/Survivor/Materials/M_Outline_Inst.M_Outline_Inst'"));
	if (TempWeaponOverlay.Succeeded())
	{
		OverlayMaterial=TempWeaponOverlay.Object;
	}

	//무기 오버레이 만들기
	WeaponOverlapBox=CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponOverlap"));
	WeaponOverlapBox->SetupAttachment(FirstCameraComp);
	WeaponOverlapBox->SetRelativeLocation(FVector(280,0,-20));
	WeaponOverlapBox->SetRelativeScale3D(FVector(10,1,1));

	WeaponOverlapBox->SetGenerateOverlapEvents(true);
	WeaponOverlapBox->SetCollisionProfileName(TEXT("WeaponBox"));

	// 실린더 메시 설정
	ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (CylinderMeshAsset.Succeeded())
	{
		CylinderMesh = CylinderMeshAsset.Object;
	}

	//BoneMap 초기화 (뼈의 이름과 우선순위 매핑))
	//TODO: None이면 예외 처리 해야함

	// 머리
    BoneMap.Add(TEXT("head"), 0);
	BoneMap.Add(TEXT("neck_01"), 0);

	// 가슴
	BoneMap.Add(TEXT("spine_02"), 1);

	// 배
	BoneMap.Add(TEXT("pelvis"), 2);
	BoneMap.Add(TEXT("spine_01"), 2);

	// 팔
	BoneMap.Add(TEXT("upperarm_l"), 3);
	BoneMap.Add(TEXT("lowerarm_l"), 3);
	BoneMap.Add(TEXT("hand_l"), 3);
	BoneMap.Add(TEXT("upperarm_r"), 3);
	BoneMap.Add(TEXT("lowerarm_r"), 3);
	BoneMap.Add(TEXT("hand_r"), 3);

	// 다리
	BoneMap.Add(TEXT("thigh_l"), 3);
	BoneMap.Add(TEXT("calf_l"), 3);
	BoneMap.Add(TEXT("foot_l"), 3);
	BoneMap.Add(TEXT("ball_l"), 3);
	BoneMap.Add(TEXT("thigh_r"), 3);
	BoneMap.Add(TEXT("calf_r"), 3);
	BoneMap.Add(TEXT("foot_r"), 3);
	BoneMap.Add(TEXT("ball_r"), 3);

}

//무기, 아이템과 박스가 오버랩 됐을때
void ASurvivor::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning,TEXT("오버랩 이벤트"));
	AWeaponBase* OverlapWeapon = Cast<AWeaponBase>(OtherActor);
	if (OverlapWeapon)
	{
		OverlapWeapon->SetOverlayMaterial(OverlayMaterial);
	}
	AItemBase* OverlapItem = Cast<AItemBase>(OtherActor);
	if (OverlapItem)
	{
		OverlapItem->SetOverlayMaterial(OverlayMaterial);
		//preset - weapon overlap과 overlap으로 체크해야함
	}
}

//무기, 아이템과 박스가 오버랩 해제
void ASurvivor::OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWeaponBase* OverlapWeapon = Cast<AWeaponBase>(OtherActor);
	if (OverlapWeapon)
	{
		OverlapWeapon->ClearOverlayMaterial();
	}
	AItemBase* OverlapItem = Cast<AItemBase>(OtherActor);
	if (OverlapItem)
	{
		OverlapItem->ClearOverlayMaterial();
	}
}

// Called when the game starts or when spawned
void ASurvivor::BeginPlay()
{
	Super::BeginPlay();
	
	//Enhanced Input
	auto* pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subSystem)
		{
			subSystem->AddMappingContext(IMC_Survivor,0);
		}
	}

	//UI붙이기
	SurvivorMainUI=Cast<UUISurvivorMain>(CreateWidget(GetWorld(),MainUIFactory));
	if (SurvivorMainUI)
	{
		SurvivorMainUI->AddToViewport();
		CurrentHP=MaxHP;
		CrosshairUI = Cast<UUISurvivorCrosshair>(SurvivorMainUI->Crosshair);
	}
	if (TakeDamageUIClass) // UPROPERTY로 설정된 UClass<UUserWidget> 변수
	{
		TakeDamageUI = CreateWidget<UUITakeDamage>(GetWorld(), TakeDamageUIClass);
		if (TakeDamageUI)
		{
			TakeDamageUI->AddToViewport();
		}
	}
	if (AttackZombieUIClass)
	{
		AttackZombieUI = CreateWidget<UUIAttackZombie>(GetWorld(), AttackZombieUIClass);
		if (AttackZombieUI)
		{
			AttackZombieUI->AddToViewport();
		}
	}
	if (MedKitUIClass)
	{
		MedKitUI = CreateWidget<UUISurvivorMedKit>(GetWorld(), MedKitUIClass);
		if (MedKitUI)
		{
			MedKitUI->AddToViewport();
		}
	}
		
	//카메라 설정
	FirstCameraComp->SetActive(true);
	ThirdPersonCameraComp->SetActive(false);
	SpringArmComp->SetActive(false);

	//BOX overlap시 발생할 이벤트
	WeaponOverlapBox->OnComponentBeginOverlap.AddDynamic(this,&ASurvivor::OnWeaponOverlap);
	WeaponOverlapBox->OnComponentEndOverlap.AddDynamic(this,&ASurvivor::OnWeaponEndOverlap);
}

// Called every frame
void ASurvivor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//direction을 캐릭터가 바라보는 방향으로 변환
	Direction=FTransform(GetControlRotation()).TransformVector(Direction);

	//이동 : 캐릭터 무브먼트 이용
	AddMovementInput(Direction);
	Direction=FVector::Zero();

	//트레이스 후 무기면 -> 줍기
	TraceForWeapon();

	// 조준선 UI 업데이트
	float Value = UKismetMathLibrary::VSize(GetVelocity());
	if (CrosshairUI)
	{
		CrosshairUI->CrosshairSpread = UKismetMathLibrary::MapRangeClamped(Value,0,450,5,70);
	}

	// 좌클릭을 꾹 누르고 있으면 회복 (5초동안 꾹누르기)
	if (bIsHoldingLeft)
	{
		HoldTime += GetWorld()->GetDeltaSeconds();
		// HoldTime이 MaxHoldTime을 초과하면 회복 및 카메라 전환
		if (HoldTime >= MaxHoldTime)
		{
			// 잃은 체력의 80% 회복
			CurrentHP += 0.8f * (MaxHP - CurrentHP);
			HoldTime = 0.0f;
			bIsHoldingLeft = false;

			// 카메라 전환
			SwitchCamera();

			// 응급도구 삭제
			if (HandleObjectSlot.WeaponFactory) 
			{
				UnequipWeapon();
				HandleObjectSlot = FWeaponData();
			}
			CurrentWeaponSlot.Reset();
			CurrentWeapon=nullptr;
		}
	}
}

// Called to bind functionality to input
void ASurvivor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto pi=CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (pi)
	{
		pi->BindAction(IA_SurMove, ETriggerEvent::Triggered, this, &ASurvivor::SurvivorMove);
		pi->BindAction(IA_SurLook, ETriggerEvent::Triggered, this, &ASurvivor::SurvivorLook);
		pi->BindAction(IA_SurTurn, ETriggerEvent::Triggered, this, &ASurvivor::SurvivorTurn);
		pi->BindAction(IA_SurJump, ETriggerEvent::Started, this, &ASurvivor::SurvivorJump);
		pi->BindAction(IA_SurCrouch, ETriggerEvent::Started, this, &ASurvivor::SurvivorCrouch);
		pi->BindAction(IA_SurFire, ETriggerEvent::Started, this, &ASurvivor::LeftClickAttack);
		pi->BindAction(IA_SurFire, ETriggerEvent::Ongoing, this, &ASurvivor::HandleHoldAttack);
		pi->BindAction(IA_SurFire, ETriggerEvent::Completed, this, &ASurvivor::HandleReleaseAttack);
		pi->BindAction(IA_SurRight, ETriggerEvent::Started, this, &ASurvivor::RightClickAttack);
		pi->BindAction(IA_SurReload, ETriggerEvent::Started, this, &ASurvivor::WeaponReload);
		pi->BindAction(IA_PrimaryWeapon, ETriggerEvent::Started, this, &ASurvivor::EquipPrimaryWeapon);
		pi->BindAction(IA_SecondaryWeapon, ETriggerEvent::Started, this, &ASurvivor::EquipSecondaryWeapon);
		pi->BindAction(IA_MeleeWeapon, ETriggerEvent::Started, this, &ASurvivor::EquipMeleeWeapon);
		pi->BindAction(IA_HandleObject, ETriggerEvent::Started, this, &ASurvivor::EquipHandleObject);
		pi->BindAction(IA_PickUp,ETriggerEvent::Started,this,&ASurvivor::PickUpWeapon_Input);
	}
}

//카메라 전환 함수 
void ASurvivor::SwitchCamera()
{
	bFirstPerson = !bFirstPerson;
	
	if (bFirstPerson)
	{
		FirstCameraComp->SetActive(true);
		ThirdPersonCameraComp->SetActive(false);
		SpringArmComp->SetActive(false);
		Arms->SetVisibility(true);
		CurrentWeapon->SetHidden(false);
		ThirdPerson->SetVisibility(false);
	}
	else
	{
		FirstCameraComp->SetActive(false);
		ThirdPersonCameraComp->SetActive(true);
		SpringArmComp->SetActive(true);
		Arms->SetVisibility(false);
		CurrentWeapon->SetHidden(true);
		ThirdPerson->SetVisibility(true);
	}
}


void ASurvivor::SurvivorMove(const struct FInputActionValue& InputValue)
{
	FVector2d dir=InputValue.Get<FVector2d>();
	Direction.X = dir.Y;
	Direction.Y = dir.X;
}

void ASurvivor::SurvivorCrouch(const struct FInputActionValue& InputValue)
{
	Crouch();
}

void ASurvivor::SurvivorTurn(const struct FInputActionValue& InputValue)
{
	float value = InputValue.Get<float>();
	AddControllerYawInput(value);
}

void ASurvivor::SurvivorLook(const struct FInputActionValue& InputValue)
{
	float value = InputValue.Get<float>();
	AddControllerPitchInput(value);
}

void ASurvivor::SurvivorJump(const struct FInputActionValue& InputValue)
{
	Jump();
}

float ASurvivor::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
							class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamaged(DamageAmount);
	return DamageAmount;
}

void ASurvivor::OnDamaged(float Damage)
{
	bIsDamaged=true;
	//카메라 쉐이크
	auto pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->PlayerCameraManager->StartCameraShake(DamagedCameraShake);
	}
	// 위젯 애니메이션 재생
	if (TakeDamageUI)
	{
		TakeDamageUI->PlayAnimationByName();
	}
	//체력깎기
	CurrentHP -= Damage;
	//0되면 ondie호출하기
	if (CurrentHP <= 0)
	{
		OnDie();
	}
}

void ASurvivor::OnDie()
{
	bIsDead = true;
	SwitchCamera();
	
}

void ASurvivor::LeftClickAttack(const struct FInputActionValue& InputValue)
{
	UAnimInstance* AnimInst = Arms->GetAnimInstance();
	USurvivorArmAnim* WeaponInst = Cast<USurvivorArmAnim>(AnimInst);
	bool bIsEquipped = WeaponInst->bIsEquippedWeapon;

	//SKYE: 슬롯을 처음에 구분후 슬롯별 무기 공격 추가 필요함
	if (bIsEquipped) //무기가 있을때만 가능
	{
		switch (CurrentWeaponSlot->WeaponName)
		{
		case EWeaponType::Primary:
			PrimaryWeaponAttack();
			break;
		case EWeaponType::Secondary:
			SecondaryWeaponAttack();
			break;
		case EWeaponType::Melee:
			MeleeWeaponAttack();
			break;
		case EWeaponType::HandleObject:
			HandleHoldAttack();
			break;
		default:
			NoneAttack();
			break;
		}
	}

	else
		NoneAttack();
}

void ASurvivor::PrimaryWeaponAttack()
{
	// 총알이 있어야....
	if (CurrentWeapon->WeaponData.CurrentAmmo > 0)
	{
		//총무기 라인트레이스
		//TODO: 나중에 따발총 추가되면 변수추가해서 바꿔야함
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		
		APlayerCameraManager* FirstCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
		if (!FirstCam)
		{
			UE_LOG(LogTemp, Error, TEXT("카메라 매니저가 없음"));
			return;
		}
		FVector Start = FirstCam->GetCameraLocation();
		FVector End = Start + (FirstCam->GetActorForwardVector() * 30000);
		    
		const float DebugLineLifetime = 2.0f;
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel6, Params);
			
		// 디버그 라인 그리기
		if (bDebugPlay)
		{
			if (bHit)
			{
				// 히트가 발생한 경우 빨간색으로 표시
				DrawDebugLine(GetWorld(), Start, Hit.Location, FColor::Red, false, DebugLineLifetime, 0, 0.5f);
				FString BoneNameStr = FString::Printf(TEXT("Hit Bone: %s"), *Hit.BoneName.ToString());
				UE_LOG(LogTemp, Warning, TEXT("%s %s"), *Hit.GetActor()->GetName(), *BoneNameStr);
			}
			else
			{
				// 히트가 없는 경우 초록색으로 표시
				DrawDebugLine(GetWorld(),Start,End,FColor::Green,false,DebugLineLifetime, 0,0.5f);
			}
		}
		    
		if (bHit && Hit.GetActor())
		{
			if (false == Hit.BoneName.IsNone())
			{
				const FVector HitFromDirection = (GetActorForwardVector() + FVector(0, 0, 0.5f)).GetSafeNormal();
				UGameplayStatics::ApplyPointDamage(Hit.GetActor(), 10, HitFromDirection, Hit, nullptr, nullptr, UDamageType::StaticClass());
			}
		}

		//총알 개수 줄이자
		DecreaseAmmoCount();

		//몽타주 플레이
		if (CurrentWeapon->WeaponData.WeaponFireMontage)
		{
			Arms->GetAnimInstance()->Montage_Play(CurrentWeapon->WeaponData.WeaponFireMontage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("무기가 없습니다"));
		}

		//카메라 쉐이크
		auto pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			pc->PlayerCameraManager->StartCameraShake(GunCameraShake);
		}
	}
}


void ASurvivor::SecondaryWeaponAttack()
{
	//근접무기 휘두르기
	Sweep();
	if (UAnimInstance* AnimInstance = Arms->GetAnimInstance())
	{
		AnimInstance->Montage_Play(SecondaryWeaponSlot.WeaponFireMontage);
	}
}

void ASurvivor::MeleeWeaponAttack()
{
	//투척무기
	//몽타주 특정시점(추가필요)에서 무기해제 (던지고 나서도 손에 들고있으면 안됨)
	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	bIsThrown = true;
	//몽타주 플레이
	if (CurrentWeapon->WeaponData.WeaponFireMontage)
	{
		Arms->GetAnimInstance()->Montage_Play(CurrentWeapon->WeaponData.WeaponFireMontage);
	}
	//SKYE: 프리셋 추가 설정
	if (bIsThrown)
	{
		CurrentWeapon->WeaponData.WeaponName=EWeaponType::None;
		ThrowWeapon();
	}
}

void ASurvivor::HandleHoldAttack()
{
	// 좌클릭을 꾹 누르고 있으면 시작
	bIsHoldingLeft = true;

	// 카메라 전환
	SwitchCamera();
}

void ASurvivor::HandleReleaseAttack()
{
	// 5초 되기전 놓기
	if (bIsHoldingLeft && HoldTime<MaxHoldTime)
	{
		HoldTime=0.0f;
		// 카메라 전환
		SwitchCamera();
		//프로그레스바 초기화
	}
	bIsHoldingLeft = false;
}

void ASurvivor::NoneAttack()
{
 	if (UAnimInstance* AnimInstance = Arms->GetAnimInstance())
	{
		AnimInstance->Montage_Play(ShoveMontage);
	}
}

void ASurvivor::Sweep()
{
	// 충돌을 위한 가상의 박스 생성
    auto BoxShape = FCollisionShape::MakeBox(FVector(100, 100, 5));
   
    // 충돌결과 저장을 위한 배열 선언
    // SweepMultiByChannel이 수행되면 여기에 HitResult 구조체(충돌과 관련된 정보들이 들어있음)들이 쌓인다
    TArray<struct FHitResult> HitResults;
  
    // 시작과 끝점 (박스의 중심), 현재의 80은 캐릭터의 머리위치 정도인듯
    // 시작 지점과 끝 지점은 같도록 하면 된다 (Z축 좌표를 모니터 정 중앙 위치를 기준으로)

	FVector Start,End;
	FRotator CameraRotation;
	FVector BoxLocation;
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		int32 ViewportX, ViewportY;
		PC->GetViewportSize(ViewportX, ViewportY);

		float CenterX = ViewportX / 2.f;
		float CenterY = ViewportY / 2.f;

		FVector WorldLocation;
		FVector WorldDirection;
		bool bDeprojectSuccess = PC->DeprojectScreenPositionToWorld(CenterX, CenterY, WorldLocation, WorldDirection);

		if (bDeprojectSuccess)
		{
			// 뷰포트 중심을 기준으로 시작점과 끝점 설정
			Start = WorldLocation;
			End = Start + (WorldDirection * 500.f); // 적당한 거리로 설정

			// 박스의 위치를 뷰포트 중심으로 설정
			BoxLocation = Start;
			CameraRotation = PC->PlayerCameraManager->GetCameraRotation();
		}
	}
	
    // 나 자신은 제외하고 싶다 (필요하면 무기 액터 같은 것들도 제외하면 된다)
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // SweepMultiByChannel 수행
    const bool bHit = GetWorld()->SweepMultiByChannel(HitResults, Start, End, CameraRotation.Quaternion(), ECC_GameTraceChannel6, BoxShape, Params);
	// ->HitResults 배열에 충돌 결과들이 저장된다

	if (bDebugPlay)
	{
		DrawDebugBox(GetWorld(), BoxLocation, FVector(100, 100, 5), CameraRotation.Quaternion(), FColor::Red, true, 3.0f);
	}

	//5. 충돌결과 처리
    // 만약 가상의 박스 안에 뭔가가 있었다면?
    if (bHit)
    {
		// 한 액터(좀비)의 여러 부위(왼쪽팔, 머리, 오른쪽팔)가 박스 영역 안에 동시에 들어왔을때 분류 (한 번만 타격!)
        TMap<AActor*, TArray<FName>> HitMap;

        // HitResults에서 각각의 HitResult를 꺼내서 확인
        for (auto HitResult : HitResults)
        {
			// 만약 BoneName이 None이라면 SkeletalMesh가 아니라는 뜻이다. 스킵하자.
            FName BoneName = HitResult.BoneName;
            if (HitResult.BoneName.IsNone())
			{
				continue;
            }

			// 좀비가 아니라면 스킵하자.
            AActor* Actor = Cast<AZombieBase>(HitResult.GetActor());
            if (nullptr == Actor)
            {
               continue;
            }

            // BoneName이 None이 아니라면
            USkeletalMeshComponent* Component = Cast<USkeletalMeshComponent>(HitResult.GetComponent());
            if (nullptr == Component)
            {
               continue;
            }

			// Actor의 BoneName 부위가 피격당했다
            // 해당 Actor가 아직 없다면 새로운 배열(TArray))을 만들고 피격부위(BoneName)를 추가한 후 HitMAp에 저장
            if (false == HitMap.Contains(Actor))
            {
               TArray<FName> BoneArray;
               BoneArray.Add(BoneName);
               HitMap.Add(Actor, BoneArray);
            }
            // 이미 존재한다면 기존의 TArray를 가져와서 BoneName을 추가한다.
            else
            {
               /*auto Array = HitMap[Actor];
               Array.Add(BoneName);
               -> Array는 복사본이므로 원본에 영향을 주지 않는다고 함*/
               HitMap[Actor].Add(BoneName); //기존 배열에 직접 추가
            }
		}

		// TMap의 Key값은 각각의 좀비를 의미한다.
        // GetKeys를 통해 어떤 좀비들이 피격을 당했는지 알아낸다.
        TArray<AActor*> Actors;
        HitMap.GetKeys(Actors);
      
        // 피격을 당한 좀비들을 하나하나 알아본다.
        for (auto Actor : Actors)
        {
			int HighPriority = INT_MAX;
            FName HighPriorityBoneName;
            // 피격 당한 부위들을 하나하나 살펴본다.
			for (auto BoneName : HitMap[Actor])
			{
				// 뼈 이름
		        int Priority = BoneMap[BoneName];
		        if (Priority < HighPriority)
				{
					HighPriority = Priority;
		            HighPriorityBoneName = BoneName;
		        }
		    }

       		// 어떤 부위들을 피격 당했는지 알았으니 우선순위가 가장 높은 Bone에 맞았다고 하고 데미지 주기
       		// 좀비는 포인트 데미지 주자
	        FHitResult HR;
	        HR.BoneName = HighPriorityBoneName;
	        UGameplayStatics::ApplyPointDamage(Actor, 9999, GetActorRightVector() * -1.0f, HR, nullptr, nullptr, nullptr);

        	//공격을 맞췄다는 변수 true
	        bIsAttacked = true;
        	AttackZombieUI->PlayAnimationByName(this);
        }
   }
	/*//카메라 쉐이크
	auto pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->PlayerCameraManager->StartCameraShake(SweepCameraShake);
	}*/
}


void ASurvivor::ThrowWeapon()
{
	FVector StartLocation = GetActorLocation() + FVector(0, 0, 80); //캐릭터 머리정도
	FVector TargetLocation = StartLocation + GetActorForwardVector()*1000;
	//0~1사이 (포물선 궤적 높이라고 이해하자)
	float arcValue = 0.5f;
	FVector outVelocity = FVector::ZeroVector;
	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, StartLocation, TargetLocation, GetWorld()->GetGravityZ(),arcValue))
	{
		FPredictProjectilePathParams predictParams (20.0f, StartLocation, outVelocity, 15.0f);
		//->20은 트레이싱이 보여질 프로젝타일 크기 /  15는 시뮬레이션 되는 Max 시간
		if (bDebugPlay)
		{
			predictParams.DrawDebugTime=15.f;
			predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;
		}
		predictParams.OverrideGravityZ=GetWorld()->GetGravityZ();
		FPredictProjectilePathResult predictResult;
		UGameplayStatics::PredictProjectilePath(this, predictParams, predictResult);

		//이제 날려보내자
		CurrentWeapon->Root->SetSimulatePhysics(true);
		CurrentWeapon->Root->SetPhysicsLinearVelocity(outVelocity);
		//근데 바닥에 닿으면 멈춰야함!
		CurrentWeapon->Root->OnComponentHit.AddDynamic(this, &ASurvivor::OnThrowWeaponHit);
		CurrentWeapon->Root->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CurrentWeapon->Root->SetCollisionResponseToAllChannels(ECR_Ignore);
		CurrentWeapon->Root->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		CurrentWeapon->Root->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		CurrentWeapon->Root->SetNotifyRigidBodyCollision(true); //충돌이벤트 발생가능하게 설정
	}
}

void ASurvivor::OnThrowWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//이미 바닥이면 실행하지말자
	if (bHasLanded) return;

	//바닥에 닿으면 속도를 0으로 만들자
	bHasLanded = true;
	
	//속도 멈추기
	CurrentWeapon->Root->SetPhysicsLinearVelocity(FVector::ZeroVector);
	CurrentWeapon->Root->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	//마찰력을 증가시키자
	CurrentWeapon->Root->SetLinearDamping(5.0f); //이속 감소
	CurrentWeapon->Root->SetAngularDamping(5.0f); //회전 감소

	//6초동안 sphere trace로 좀비 감지하기 (범위 3000이상)
	//0.2초마다 loop
	GetWorld()->GetTimerManager().SetTimer(PipeBombTraceTimerHandle, this, &ASurvivor::PipeBombTraceZombies, 0.1f, true);

	//6초후 폭발 타이머 설정 (폭발함수 구현하기)
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &ASurvivor::ExplodeWeapon, 6.0f, false);
	
}

void ASurvivor::PipeBombTraceZombies()
{
	//Sphere Trace 실행
	FVector TraceStart = CurrentWeapon->GetActorLocation();
	FVector TraceEnd = TraceStart;
	float TraceRadius = 3000.0f; //감지 범위

	TArray<FHitResult> HitResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(TraceRadius);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_GameTraceChannel6,
		SphereShape
	);

	//감지된 액터들 중 ACommonZombie 타입인지 확인하고 이벤트 호출
	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->IsA(ACommonZombie::StaticClass()))
		{
			ACommonZombie* Zombie = Cast<ACommonZombie>(HitActor);
			if (Zombie)
			{
				Zombie->HandlePipeBombBeep(CurrentWeapon);
			}
		}
	}
}

void ASurvivor::ExplodeWeapon()
{
	//좀비 감지 타이머 중지
	GetWorld()->GetTimerManager().ClearTimer(PipeBombTraceTimerHandle);
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	//데미지 적용
	UGameplayStatics::ApplyRadialDamage(
		this,500.f,
		CurrentWeapon->GetActorLocation(),
		500.f,
		UDamageType::StaticClass(),
		IgnoreActors,
		this,
		GetWorld()->GetFirstPlayerController(),
		true,
		ECC_GameTraceChannel6);

	//폭발 반경을 빨간색 구체로 표시 (2초간 유지)
	if (bDebugPlay)
	{
		DrawDebugSphere(
			GetWorld(),           // World
			CurrentWeapon->GetActorLocation(),    // Center
			500.f,      // Radius
			32,                   // Segments
			FColor::Red,          // Color
			false,               // Persistent Lines
			2.0f);                // Duration
	}

	CurrentWeapon->Destroy();
	CurrentWeapon==nullptr;
}

//장전
void ASurvivor::WeaponReload(const struct FInputActionValue& InputValue)
{
	//총일때만 가능합니다
	if (CurrentWeapon && CurrentWeapon->Name == EWeaponType::Primary)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASurvivor::WeaponReload"));

		// 총알이 아직 부족하고, 남은 탄약이 존재하는 경우에만 장전 가능
		if (CurrentWeapon->WeaponData.CurrentAmmo < CurrentWeapon->WeaponData.MaxAmmo &&
			CurrentWeapon->WeaponData.MaxAmmoAmount > 0)
		{
			// 현재 총기에 남아있는 탄약과 필요한 탄약 계산
			int32 NeededAmmo = CurrentWeapon->WeaponData.MaxAmmo - CurrentWeapon->WeaponData.CurrentAmmo;
			// gpt의 정리...
			int32 AmmoToLoad = FMath::Min(NeededAmmo, CurrentWeapon->WeaponData.MaxAmmoAmount);

			// 장전된 탄 수 반영
			CurrentWeapon->WeaponData.CurrentAmmo += AmmoToLoad;
			CurrentWeapon->WeaponData.MaxAmmoAmount -= AmmoToLoad;
		}
	}

	// 몽타주 플레이
	if (UAnimInstance* AnimInstance = Arms->GetAnimInstance())
	{
		AnimInstance->Montage_Play(CurrentWeapon->WeaponData.WeaponReloadMontage);
	}
}

void ASurvivor::DecreaseAmmoCount()
{
	if (CurrentWeapon->WeaponData.CurrentAmmo > 0)
	{
		CurrentWeapon->WeaponData.CurrentAmmo--;
	}
}


//우클릭시 밀쳐내기
void ASurvivor::RightClickAttack(const struct FInputActionValue& InputValue)
{
    if (UAnimInstance* AnimInstance = Arms->GetAnimInstance())
    {
        AnimInstance->Montage_Play(ShoveMontage);
    }
}

void ASurvivor::OnShoveOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    ACommonZombie* CommonZombie = Cast<ACommonZombie>(OtherActor);
	//조건1: 좀비가 맞았는지?
    if (CommonZombie)
    {
    	UE_LOG(LogTemp, Warning, TEXT("오버랩발생"));
        //조건2: 좀비가 플레이어 전방 기준 좌우 45도 안에 있는가?
        FVector ZombieLocation = (CommonZombie->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FVector SurvivorForwardVector = GetActorForwardVector();

    	//내적으로 각도 구하기
        float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(SurvivorForwardVector, ZombieLocation)));
        if (Angle <= 45.0f)
        {
            //피해 적용
            UGameplayStatics::ApplyDamage(CommonZombie, 10, GetController(), this, UShoveDamageType::StaticClass());
        }
    }
}

void ASurvivor::DestroyShoveCylinder()
{
	if (ShoveCollisionCylinder)
	{
		ShoveCollisionCylinder->DestroyComponent();
		ShoveCollisionCylinder = nullptr;
	}
}

void ASurvivor::spawnShoveCylinder()
{
	if (ShoveCollisionCylinder)
	{
		ShoveCollisionCylinder->DestroyComponent();
		ShoveCollisionCylinder = nullptr;
	}

	//Static Mesh가 정상적으로 로드되었는지 확인
	if (!CylinderMesh)
	{
		return;
	}

	//새로운 Collision Cylinder 생성
	ShoveCollisionCylinder = NewObject<UStaticMeshComponent>(this);
	if (!ShoveCollisionCylinder)
	{
		return;
	}

	ShoveCollisionCylinder->RegisterComponent();
	ShoveCollisionCylinder->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ShoveCollisionCylinder->SetGenerateOverlapEvents(true);
	ShoveCollisionCylinder->OnComponentBeginOverlap.AddDynamic(this, &ASurvivor::OnShoveOverlap);
	ShoveCollisionCylinder->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	ShoveCollisionCylinder->SetStaticMesh(CylinderMesh);

	//위치 및 크기 설정
	FVector StartLocation = FirstCameraComp->GetComponentLocation();
	FVector CylinderLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z-15);
	ShoveCollisionCylinder->SetWorldLocation(CylinderLocation);
	ShoveCollisionCylinder->SetWorldScale3D(FVector(3, 3, 0.2));
	
	FRotator CameraRotation = FirstCameraComp->GetComponentRotation();
	ShoveCollisionCylinder->SetWorldRotation(CameraRotation);

	ShoveCollisionCylinder->SetVisibility(false);
	GetWorld()->GetTimerManager().SetTimer(CylinderTimerHandle, this, &ASurvivor::DestroyShoveCylinder, 0.1f, false);
}


//무기 슬롯 설정 (1,2,3,4 키 바인딩)
void ASurvivor::EquipPrimaryWeapon(const struct FInputActionValue& InputValue)
{
	if (PrimaryWeaponSlot.WeaponFactory)
	{
		EquipWeapon(&PrimaryWeaponSlot);
	}
}

void ASurvivor::EquipSecondaryWeapon(const struct FInputActionValue& InputValue)
{
	if (SecondaryWeaponSlot.WeaponFactory)
	{
		EquipWeapon(&SecondaryWeaponSlot);
	}
}

void ASurvivor::EquipMeleeWeapon(const struct FInputActionValue& InputValue)
{
	if (MeleeWeaponSlot.WeaponFactory)
	{
		EquipWeapon(&MeleeWeaponSlot);
	}
}

void ASurvivor::EquipHandleObject(const struct FInputActionValue& InputValue)
{
	if (HandleObjectSlot.WeaponFactory)
	{
		EquipWeapon(&HandleObjectSlot);
	}
}

//무기 슬롯값 애니메이션 구분하기
int32 ASurvivor::GetCurrentWeaponSlotIndex() const
{
	if (!CurrentWeaponSlot.IsSet())
	{
		return -1; // 장착된 무기가 없을 경우 -1 반환
	}

	const FWeaponData& CurrentWeaponData = CurrentWeaponSlot.GetValue();

	if (CurrentWeaponData.WeaponName == EWeaponType::Primary)
	{
		return 0; // Primary 슬롯
	}
	else if (CurrentWeaponData.WeaponName == EWeaponType::Secondary)
	{
		return 1; // Secondary 슬롯
	}
	else if (CurrentWeaponData.WeaponName == EWeaponType::Melee)
	{
		return 2; // Melee 슬롯
	}
	else if (CurrentWeaponData.WeaponName == EWeaponType::HandleObject)
	{
		return 3; // Handle 슬롯
	}

	return -1; // 알 수 없는 슬롯
}

//무기 발견하기 (카메라 라인트레이스)
void ASurvivor::TraceForWeapon()
{
	FVector Start = FirstCameraComp->GetComponentLocation(); // 카메라 위치
	FVector ForwardVector = FirstCameraComp->GetForwardVector(); // 카메라의 정면 방향
	FVector End = Start + (ForwardVector * 1000.f); // 1000cm(10m) 앞까지 탐색

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 자기 자신은 무시
	
	if (CurrentWeapon)
	{
		Params.AddIgnoredActor(CurrentWeapon);
	}

	const float CapsuleRadius = 30.0f; // 캡슐의 반지름 설정
	const float CapsuleHalfHeight = 50.0f; // 캡슐의 반 높이 설정
	const float DebugLineLifetime = 2.0f;

	// Capsule Trace 실행
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity, // 회전 없음
		ECC_Visibility, // 충돌 채널
		FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), // 캡슐 형태
		Params
	);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		
		AWeaponBase* HitWeapon = Cast<AWeaponBase>(HitResult.GetActor()); // 무기인지 확인
		AItemBase* HitItem = Cast<AItemBase>(HitResult.GetActor()); // 아이템인지 확인

		if (HitWeapon)
		{
			// 이전에 포커스된 무기가 있고, 현재 포커스된 무기와 다르다면 이전 무기의 머티리얼을 복원
			FocusedWeapon = HitWeapon; // 감지한 무기를 저장
		}
		else if (HitItem)
		{
			FocusedItem = HitItem;
		}
		else
		{
			FocusedWeapon = nullptr; // 무기가 아니면 초기화
			FocusedItem = nullptr;
		}
	}
	else
	{
		FocusedWeapon = nullptr;
		FocusedItem = nullptr; //감지된게 없으면 초기화
	}
}

//무기줍기 (E) 
void ASurvivor::PickUpWeapon_Input(const FInputActionValue& Value)
{
	if (FocusedWeapon) // 플레이어가 바라보고 있는 무기가 있다면
	{
		PickUpWeapon(FocusedWeapon->WeaponData);
	}
	if (FocusedItem)
	{
		FocusedItem->Interaction();
		UE_LOG(LogTemp, Warning, TEXT("아이템 상호작용"));
	}
}

void ASurvivor::PickUpWeapon(FWeaponData NewWeapon)
{
	//장착할 슬롯을 결정
	switch (NewWeapon.WeaponName)
	{
	case EWeaponType::Primary:
		if (CurrentWeapon) // 이미 무기가 있다면 교체
		{
			UnequipWeapon();
		}
		PrimaryWeaponSlot = NewWeapon;
		EquipWeapon(&PrimaryWeaponSlot);
		break;

	case EWeaponType::Secondary:
		if (CurrentWeapon) // 이미 무기가 있다면 교체
		{
			UnequipWeapon();
		}
		SecondaryWeaponSlot = NewWeapon;
		EquipWeapon(&SecondaryWeaponSlot);
		break;

	case EWeaponType::Melee:
		if (CurrentWeapon) // 이미 무기가 있다면 교체
		{
			UnequipWeapon();
		}
		MeleeWeaponSlot = NewWeapon;
		bIsThrown = false;
		EquipWeapon(&MeleeWeaponSlot);
		break;

	case EWeaponType::HandleObject:
		if (CurrentWeapon) // 이미 무기가 있다면 교체
		{
			UnequipWeapon();
		}
		HandleObjectSlot = NewWeapon;
		EquipWeapon(&HandleObjectSlot);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("무기 모르겠어용"));
		break;
	}
}

//무기 장착
void ASurvivor::EquipWeapon(FWeaponData* WeaponData)
{
	if (!WeaponData || !WeaponData->WeaponFactory) return;
	// 이미 같은 무기를 들고 있다면 무시
	if (CurrentWeapon && CurrentWeapon->GetClass() == WeaponData->WeaponFactory->GetDefaultObject()->GetClass())
	{
		return;
	}

	// 기존 무기가 있다면 숨기고 장착 해제
	if (CurrentWeapon)
	{
		CurrentWeapon->SetEquipped(false);
		CurrentWeapon->SetActorHiddenInGame(true);
		UAnimInstance* AnimInst = Arms->GetAnimInstance();
	}

	// 월드에 있는 무기를 찾음
	AWeaponBase* WorldWeapon = FindWeaponInWorld(WeaponData);
	if (WorldWeapon)
	{
		// 월드에 있는 무기를 장착
		//SKYE: 무기 프리셋 변경1
		WorldWeapon->SetEquipped(true);
		WorldWeapon->SetActorHiddenInGame(false);
		CurrentWeapon = WorldWeapon;
		WorldWeapon->Root->SetCollisionProfileName(TEXT("EquipWeapon"));
	}
	else
	{
		// 월드에 무기가 없으면 새로 생성
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponData->WeaponFactory);
		CurrentWeapon->SetEquipped(true);
	}

	// 현재 장착한 무기 데이터 업데이트
	CurrentWeaponSlot = *WeaponData;
	
	// 무기를 캐릭터의 소켓에 부착
	if (CurrentWeapon && Arms)
	{
		CurrentWeapon->AttachToComponent(Arms, FAttachmentTransformRules::KeepRelativeTransform, "WeaponSocket");
		CurrentWeapon->SetActorRelativeRotation(FRotator(0, 0, 0));
		CurrentWeapon->SetActorRelativeLocation(FVector(0, 0, 0));
		
		// WeaponName enum을 int로 변환하여 슬롯 인덱스로 사용
		int32 SlotIndex = static_cast<int32>(CurrentWeapon->WeaponData.WeaponName)-1;

		// WeaponInstance (weapon data인거죵) 를 배열에 저장
		if (WeaponInstances.Num() <= SlotIndex)
		{
			WeaponInstances.SetNum(SlotIndex + 1);
		}
		WeaponInstances[SlotIndex] = CurrentWeapon->WeaponData;
    
		// 수정된 WeaponInstance 배열을 전달
		SurvivorMainUI->WeaponSlot->UpdateSlot(SlotIndex, WeaponInstances);
	}
	
	UAnimInstance* AnimInst = Arms->GetAnimInstance();
	if (AnimInst)
	{
		USurvivorArmAnim* WeaponInst = Cast<USurvivorArmAnim>(AnimInst);
		if (WeaponInst)
		{
			WeaponInst->bIsEquippedWeapon=true;
		}
	}
	
	if (WeaponData->WeaponDrawMontage)
	{
		Arms->GetAnimInstance()->Montage_Play(WeaponData->WeaponDrawMontage);
	}
}


//월드에 소환된 무기중에 false값을 가지고 있는 무기만 찾아주기
AWeaponBase* ASurvivor::FindWeaponInWorld(FWeaponData* WeaponData)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponBase::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AWeaponBase* Weapon = Cast<AWeaponBase>(Actor);
		if (Weapon && Weapon->GetClass() == WeaponData->WeaponFactory->GetDefaultObject()->GetClass() && !Weapon->IsEquipped)
		{
			return Weapon;
		}
	}
	return nullptr;
}

//무기 내리기
void ASurvivor::UnequipWeapon()
{
	if (CurrentWeapon)
	{
		//SKYE: 무기 프리셋 변경2
		CurrentWeapon->Root->SetCollisionProfileName(TEXT("WorldWeapon"));
		CurrentWeapon->SetActorHiddenInGame(true);
	}
	CurrentWeaponSlot.Reset();

	if (UnequipMontage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(UnequipMontage);
	}

	// 애니메이션 인스턴스 업데이트
	UAnimInstance* AnimInst = Arms->GetAnimInstance();
	if (AnimInst)
	{
		USurvivorArmAnim* WeaponInst = Cast<USurvivorArmAnim>(AnimInst);
		if (WeaponInst)
		{
			WeaponInst->bIsEquippedWeapon = false;
		}
	}
}





