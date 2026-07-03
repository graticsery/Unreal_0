// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Projectile.h"
#include "HyperFPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

// 생성자
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// ========================================
	// Weapon Mesh 생성
	// ========================================
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// ========================================
	// 기본값 설정
	// ========================================
	CurrentAmmo = MagazineCapacity;
	OwnerCharacter = nullptr;
}

// 게임 시작 시 호출
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	// 탄약 초기화
	CurrentAmmo = MagazineCapacity;

	// 보유 캐릭터 참조 저장
	OwnerCharacter = Cast<AHyperFPSCharacter>(GetInstigator());
}

// 매 프레임 호출
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ========================================
// 사격 함수들
// ========================================

// 사격 시작 (좌클릭 눌림)
void AWeaponBase::StartFiring()
{
	if (bIsReloading) return;

	bIsFiring = true;
	bCanFire = true;

	// FullBody를 FALSE로 설정 (상체 애니메이션 활성화)
	if (AHyperFPSCharacter* Character = Cast<AHyperFPSCharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				// FullBody 설정
				FBoolProperty* FullBodyProp = FindFProperty<FBoolProperty>(
					AnimInstance->GetClass(),
					FName("FullBody")
				);

				if (FullBodyProp)
				{
					bool bFalse = false;
					FullBodyProp->SetPropertyValue_InContainer(AnimInstance, bFalse);
					UE_LOG(LogTemp, Warning, TEXT("FullBody set to FALSE"));
				}

				// IsAttacking도 FALSE로 설정
				FBoolProperty* IsAttackingProp = FindFProperty<FBoolProperty>(
					AnimInstance->GetClass(),
					FName("IsAttacking")
				);

				if (IsAttackingProp)
				{
					bool bFalse = false;
					IsAttackingProp->SetPropertyValue_InContainer(AnimInstance, bFalse);
					UE_LOG(LogTemp, Warning, TEXT("IsAttacking set to FALSE"));
				}
			}
		}
	}

	Fire();

	GetWorldTimerManager().SetTimer(
		FireRateTimerHandle,
		this,
		&AWeaponBase::Fire,
		FireRate,
		true
	);
}



// 사격 종료 (좌클릭 떼기)
void AWeaponBase::StopFiring()
{
	bIsFiring = false;

	// FullBody를 TRUE로 설정 (전신 애니메이션)
	if (AHyperFPSCharacter* Character = Cast<AHyperFPSCharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				FBoolProperty* FullBodyProp = FindFProperty<FBoolProperty>(
					AnimInstance->GetClass(),
					FName("FullBody")
				);

				if (FullBodyProp)
				{
					bool bTrue = true;
					FullBodyProp->SetPropertyValue_InContainer(AnimInstance, bTrue);
					UE_LOG(LogTemp, Warning, TEXT("FullBody set to TRUE"));
				}

				FBoolProperty* IsAttackingProp = FindFProperty<FBoolProperty>(
					AnimInstance->GetClass(),
					FName("IsAttacking")
				);

				if (IsAttackingProp)
				{
					bool bTrue = true;
					IsAttackingProp->SetPropertyValue_InContainer(AnimInstance, bTrue);
					UE_LOG(LogTemp, Warning, TEXT("IsAttacking set to TRUE"));
				}
			}
		}
	}

	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}




// 단발 사격
void AWeaponBase::Fire()
{
	if (CurrentAmmo <= 0)
	{
		
		return;
	}

	if (ProjectileClass != nullptr && GetOwner() != nullptr)
	{
		AHyperFPSCharacter* Character = Cast<AHyperFPSCharacter>(GetOwner());
		if (!Character) return;

		FVector CameraLocation;
		FRotator CameraRotation;
		Character->GetActorEyesViewPoint(CameraLocation, CameraRotation);

		FVector MuzzleLocation = CameraLocation + CameraRotation.RotateVector(FVector(100.0f, 0.0f, 0.0f));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Character;

		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass,
			MuzzleLocation,
			CameraRotation,
			SpawnParams
		);

		if (Projectile)
		{
			CurrentAmmo--;
		}
	}
}


// ========================================
// 재장전 함수들
// ========================================

// 재장전 시작
void AWeaponBase::StartReload()
{
	if (!CanReload()) return;

	bIsReloading = true;

	// 재장전 사운드
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ReloadSound,
			GetActorLocation()
		);
	}

	// 재장전 타이머 시작
	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&AWeaponBase::FinishReload,
		ReloadTime,
		false
	);
}

// 재장전 완료
void AWeaponBase::FinishReload()
{
	CurrentAmmo = MagazineCapacity;
	bIsReloading = false;
}

// ========================================
// 상태 확인 함수들
// ========================================

// 사격 가능한가?
bool AWeaponBase::CanFire() const
{
	return CurrentAmmo > 0 && !bIsReloading;
}

// 재장전 가능한가?
bool AWeaponBase::CanReload() const
{
	return CurrentAmmo < MagazineCapacity && !bIsReloading;
}
// WeaponBase.cpp에 추가

// WeaponBase.cpp 끝에 추가

float AWeaponBase::GetReloadPercent() const
{
	// 재장전 중이 아니면 0%
	if (!bIsReloading) return 0.0f;

	// 타이머가 활성화되지 않았으면 0%
	if (!GetWorldTimerManager().IsTimerActive(ReloadTimerHandle)) return 0.0f;

	// 경과 시간 / 총 재장전 시간 = 진행도
	float Remaining = GetWorldTimerManager().GetTimerRemaining(ReloadTimerHandle);
	float Elapsed = ReloadTime - Remaining;

	return FMath::Clamp(Elapsed / ReloadTime, 0.0f, 1.0f);
}
