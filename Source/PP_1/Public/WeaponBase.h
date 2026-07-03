// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

// Forward Declarations
class USkeletalMeshComponent;
class AProjectile;
class AHyperFPSCharacter;

/**
 * 총기 기본 클래스
 * 사격, 탄약 관리, 재장전 기능을 담당합니다.
 */
UCLASS()
class PP_1_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ========================================
	// 컴포넌트
	// ========================================

	/** 총기 메쉬 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	// ========================================
	// 총기 설정
	// ========================================

	/** 발사체 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Settings")
	TSubclassOf<AProjectile> ProjectileClass;

	/** 발사 속도 (초당 발사 횟수) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Settings")
	float FireRate = 0.1f;

	/** 총구 위치 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Settings")
	FVector MuzzleOffset = FVector(100.0f, 0.0f, 0.0f);

	// ========================================
	// 탄약 설정
	// ========================================

	/** 탄창 용량 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	int32 MagazineCapacity = 30;

	/** 현재 탄약 수 */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmo = 30;

	// ========================================
	// 재장전 설정
	// ========================================

	/** 재장전 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Reload")
	float ReloadTime = 2.0f;

	/** 재장전 중인가? */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Reload")
	bool bIsReloading = false;



	// ========================================
	// 사운드
	// ========================================

	/** 발사 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Sound")
	USoundBase* FireSound;

	/** 재장전 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Sound")
	USoundBase* ReloadSound;

	// ========================================
	// 이펐트
	// ========================================

	/** 머즐 플래시 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
	UParticleSystem* MuzzleFlashEffect;

	// ========================================
	// 함수
	// ========================================

	/** 사격 시작 (좌클릭 눌림) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFiring();

	/** 사격 종료 (좌클릭 떼기) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFiring();

	/** 단발 사격 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	/** 재장전 시작 (R 키) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartReload();

	/** 재장전 완료 */
	UFUNCTION()
	void FinishReload();

	/** 사격 가능한가? */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool CanFire() const;

	/** 재장전 가능한가? */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool CanReload() const;

	// WeaponBase.h의 public 섹션에 추가

/** 현재 탄약 반환 */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	/** 최대 탄약 반환 */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetMaxAmmo() const { return MagazineCapacity; }

	// WeaponBase.h의 public 섹션에 추가

/** 재장전 중인지 */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsReloading() const { return bIsReloading; }

	/** 재장전 진행도 (0.0 ~ 1.0) */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetReloadPercent() const;



private:
	// ========================================
	// 내부 변수
	// ========================================
    /** 사격 중인가? */
	bool bIsFiring = false;

	/** 발사 가능 여부 */
	bool bCanFire = true;

	/** 재장전 타이머 핸들 */
	FTimerHandle ReloadTimerHandle;

	/** 연속 사격 타이머 핸들 */
	FTimerHandle FireTimerHandle;

	/** 연사 타이머 핸들 */
	FTimerHandle FireRateTimerHandle;

	/** 보유 캐릭터 참조 */
	AHyperFPSCharacter* OwnerCharacter;
};