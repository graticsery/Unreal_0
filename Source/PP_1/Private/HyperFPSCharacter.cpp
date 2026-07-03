// Fill out your copyright notice in the Description page of Project Settings.

#include "HyperFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "WeaponBase.h"
#include "SpecialProjectile.h"
#include "TargetDummy.h"
#include "Kismet/GameplayStatics.h"  // GetAllActorsWithTag

// 생성자
AHyperFPSCharacter::AHyperFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ========================================
	// 기본값 설정
	// ========================================
	WalkSpeed = 600.0f;
	JumpZVelocity = 600.0f;
	CameraDistance = 300.0f;
	CameraLocationOffset = FVector(0.0f, 50.0f, 60.0f);
	LookSensitivityX = 1.0f;
	LookSensitivityY = 1.0f;
	PitchMin = -80.0f;
	PitchMax = 80.0f;
	CurrentWeapon = nullptr;

	// ========================================
	// Capsule 설정
	// ========================================
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// ========================================
	// Character Movement 설정
	// ========================================
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// ========================================
	// Mesh 설정
	// ========================================
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -96.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// ========================================
	// Spring Arm 생성
	// ========================================
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CameraDistance;
	CameraBoom->SetRelativeLocation(CameraLocationOffset);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;
	CameraBoom->bDoCollisionTest = true;

	// ========================================
	// Camera 생성
	// ========================================
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ========================================
	// Controller 회전 설정
	// ========================================
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// 게임 시작 시 호출
void AHyperFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ========================================
	// Enhanced Input 설정
	// ========================================
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// 이동 설정 적용
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;

	// ========================================
	// 총기 스폰
	// ========================================
	if (WeaponClass != nullptr)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator = this;
		SpawnParameters.Owner = this;

		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(
			WeaponClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParameters
		);

		// 총기를 캐릭터에 붙이기
		if (CurrentWeapon)
		{
			CurrentWeapon->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		}
	}

	// 특수 탄환 초기화
	SpecialAmmo = MaxSpecialAmmo;
	StartSpecialAmmoRecharge();

}

// 매 프레임 호출
void AHyperFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 입력 바인딩 설정
void AHyperFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동 입력
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHyperFPSCharacter::Move);
		}

		// 시점 회전 입력
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHyperFPSCharacter::Look);
		}

		// 점프 입력
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHyperFPSCharacter::StartJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHyperFPSCharacter::StopJump);
		}

		// 사격 입력
		
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AHyperFPSCharacter::StartFiring);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AHyperFPSCharacter::StopFiring);
		}
		

		// 재장전 입력
		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AHyperFPSCharacter::Reload);
		}
	}
}

// ========================================
// 입력 처리 함수들
// ========================================

// WASD 이동
void AHyperFPSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

// 마우스 시점 회전
void AHyperFPSCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * LookSensitivityX);

		FRotator CurrentRotation = GetControlRotation();
		float CurrentPitch = CurrentRotation.Pitch;
		if (CurrentPitch > 180.0f) CurrentPitch -= 360.0f;

		float NewPitch = CurrentPitch + (LookAxisVector.Y * LookSensitivityY);
		NewPitch = FMath::Clamp(NewPitch, PitchMin, PitchMax);

		AddControllerPitchInput(NewPitch - CurrentPitch);
	}
}

// 점프 시작
void AHyperFPSCharacter::StartJump()
{
	Jump();
}

// 점프 끝
void AHyperFPSCharacter::StopJump()
{
	StopJumping();
}

// 사격 시작
void AHyperFPSCharacter::StartFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFiring();
	}
}

// 사격 종료
void AHyperFPSCharacter::StopFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

// 재장전
void AHyperFPSCharacter::Reload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}

void AHyperFPSCharacter::FireSpecialProjectile()
{
	// 특수 탄환이 없으면 리턴
	if (SpecialAmmo <= 0) return;

	// 무기가 없으면 리턴
	if (!CurrentWeapon) return;

	// 특수 탄환 클래스가 없으면 리턴
	if (!SpecialProjectileClass) return;

	// 발사 위치
	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);

	FVector MuzzleLocation = CameraLocation + CameraRotation.RotateVector(FVector(100.0f, 0.0f, 0.0f));

	// 발사체 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
		SpecialProjectileClass,
		MuzzleLocation,
		CameraRotation,
		SpawnParams
	);

	if (Projectile)
	{
		// 특수 탄환 소모
		SpecialAmmo--;

		// 충전 시작 (특수 탄환이 최대치가 아니면)
		if (SpecialAmmo < MaxSpecialAmmo && !GetWorldTimerManager().IsTimerActive(SpecialAmmoRechargeHandle))
		{
			StartSpecialAmmoRecharge();
		}
	}
}

void AHyperFPSCharacter::StartSpecialAmmoRecharge()
{
	// 이미 최대치면 리턴
	if (SpecialAmmo >= MaxSpecialAmmo) return;

	// 타이머 시작
	GetWorldTimerManager().SetTimer(
		SpecialAmmoRechargeHandle,
		this,
		&AHyperFPSCharacter::OnSpecialAmmoRecharged,
		SpecialAmmoRechargeTime,
		false
	);
}

void AHyperFPSCharacter::OnSpecialAmmoRecharged()
{
	// 특수 탄환 충전
	SpecialAmmo++;

	// 최대치가 아니면 다시 충전 시작
	if (SpecialAmmo < MaxSpecialAmmo)
	{
		StartSpecialAmmoRecharge();
	}
}

// ========================================
// Q 스킬 - 기폭
// ========================================

void AHyperFPSCharacter::UseQSkill()
{
	if (!bCanUseQSkill) return;

	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Marked"), OverlappingActors);

	UE_LOG(LogTemp, Warning, TEXT("Found %d marked actors"), OverlappingActors.Num());

	int32 DetonatedCount = 0;

	for (AActor* Actor : OverlappingActors)
	{
		float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());

		UE_LOG(LogTemp, Warning, TEXT("Actor: %s, Distance: %f, Radius: %f"),
			*Actor->GetName(), Distance, DetonationRadius);

		if (Distance <= DetonationRadius)
		{
			// 표식 개수 카운트
			int32 MarkCount = 0;
			for (const FName& Tag : Actor->Tags)
			{
				if (Tag == FName("Marked"))
				{
					MarkCount++;
				}
			}

			// 표식 개수에 비례한 데미지
			float TotalDamage = DetonationDamage * MarkCount;

			UE_LOG(LogTemp, Warning, TEXT("Actor: %s, Marks: %d, Damage: %f"),
				*Actor->GetName(), MarkCount, TotalDamage);

			// ========================================
			// 폭발 이펙트 스폰
			// ========================================
			if (QSkillExplosionEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					QSkillExplosionEffect,
					Actor->GetActorLocation(),
					FRotator::ZeroRotator,
					FVector(2.0f)  // 크기 2배
				);
			}

			// 데미지 적용
			UGameplayStatics::ApplyDamage(
				Actor,
				TotalDamage,
				GetController(),
				this,
				nullptr
			);

			// 모든 표식 제거
			Actor->Tags.RemoveAll([](const FName& Tag) {
				return Tag == FName("Marked");
				});

			// 표식 Widget 제거
			if (ATargetDummy* TargetDummy = Cast<ATargetDummy>(Actor))
			{
				for (UUserWidget* Widget : TargetDummy->MarkWidgets)
				{
					if (Widget)
					{
						Widget->RemoveFromParent();
					}
				}
				TargetDummy->MarkWidgets.Empty();
				UE_LOG(LogTemp, Warning, TEXT("Removed all mark widgets from: %s"), *Actor->GetName());
			}

			DetonatedCount++;

			UE_LOG(LogTemp, Warning, TEXT("Detonated: %s with %d marks"),
				*Actor->GetName(), MarkCount);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Total detonated: %d"), DetonatedCount);

	if (DetonatedCount > 0)
	{
		bCanUseQSkill = false;

		GetWorldTimerManager().SetTimer(
			QSkillCooldownHandle,
			this,
			&AHyperFPSCharacter::OnQSkillCooldownFinished,
			QSkillCooldown,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No targets detonated!"));
	}
}
void AHyperFPSCharacter::OnQSkillCooldownFinished()
{
	bCanUseQSkill = true;
}
// ========================================
// E 스킬 - 대쉬
// ========================================

// HyperFPSCharacter.cpp의 UseESkill 함수 수정

void AHyperFPSCharacter::UseESkill()
{
	if (!bCanUseESkill) return;

	// 대쉬 방향
	FVector DashDirection = GetLastMovementInputVector();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}
	DashDirection.Normalize();

	// ========================================
	// 대쉬 이펙트 스폰
	// ========================================
	if (ESkillDashEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ESkillDashEffect,
			GetActorLocation(),
			DashDirection.Rotation(),
			FVector(1.5f)  // 크기 1.5배
		);
	}

	// 대쉬 실행
	FVector LaunchVelocity = DashDirection * DashSpeed;
	LaunchCharacter(LaunchVelocity, true, true);

	// 쿨다운 시작
	bCanUseESkill = false;

	GetWorldTimerManager().SetTimer(
		ESkillCooldownHandle,
		this,
		&AHyperFPSCharacter::OnESkillCooldownFinished,
		ESkillCooldown,
		false
	);
}

void AHyperFPSCharacter::OnESkillCooldownFinished()
{
	bCanUseESkill = true;
}
// HyperFPSCharacter.cpp에 추가

// HyperFPSCharacter.cpp 끝에 추가

float AHyperFPSCharacter::GetSpecialAmmoRechargePercent() const
{
	// 최대치면 0% (충전 안 함)
	if (SpecialAmmo >= MaxSpecialAmmo) return 0.0f;

	// 타이머가 활성화되지 않았으면 0%
	if (!GetWorldTimerManager().IsTimerActive(SpecialAmmoRechargeHandle)) return 0.0f;

	// 경과 시간 / 총 충전 시간 = 진행도
	float Remaining = GetWorldTimerManager().GetTimerRemaining(SpecialAmmoRechargeHandle);
	float Elapsed = SpecialAmmoRechargeTime - Remaining;

	return FMath::Clamp(Elapsed / SpecialAmmoRechargeTime, 0.0f, 1.0f);
}

// HyperFPSCharacter.cpp 끝에 추가

float AHyperFPSCharacter::GetQSkillCooldownPercent() const
{
	// 사용 가능하면 0% (쿨다운 없음)
	if (bCanUseQSkill) return 0.0f;

	// 타이머가 활성화되지 않았으면 0%
	if (!GetWorldTimerManager().IsTimerActive(QSkillCooldownHandle)) return 0.0f;

	// 남은 시간 / 총 쿨다운 시간 = 진행도
	float Remaining = GetWorldTimerManager().GetTimerRemaining(QSkillCooldownHandle);

	return FMath::Clamp(Remaining / QSkillCooldown, 0.0f, 1.0f);
}

// HyperFPSCharacter.cpp 끝에 추가

float AHyperFPSCharacter::GetESkillCooldownPercent() const
{
	// 사용 가능하면 0% (쿨다운 없음)
	if (bCanUseESkill) return 0.0f;

	// 타이머가 활성화되지 않았으면 0%
	if (!GetWorldTimerManager().IsTimerActive(ESkillCooldownHandle)) return 0.0f;

	// 남은 시간 / 총 쿨다운 시간 = 진행도
	float Remaining = GetWorldTimerManager().GetTimerRemaining(ESkillCooldownHandle);

	return FMath::Clamp(Remaining / ESkillCooldown, 0.0f, 1.0f);
}