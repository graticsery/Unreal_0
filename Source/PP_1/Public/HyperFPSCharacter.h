// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Projectile.h"
#include "HyperFPSCharacter.generated.h"


// Forward Declarations
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AWeaponBase;

/**
 * PP_1 프로젝트 메인 캐릭터 클래스
 */
UCLASS()
class PP_1_API AHyperFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHyperFPSCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// HyperFPSCharacter.h의 public 섹션에 추가


	// ========================================
	// 컴포넌트
	// ========================================

	/** 카메라를 캐릭터로부터 떨어뜨리는 스프링 암 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	/** 캐릭터를 따라가는 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	// ========================================
	// Enhanced Input
	// ========================================

	/** 입력 매핑 컨텍스트 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	/** 이동 입력 액션 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	/** 시점 회전 입력 액션 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	/** 점프 입력 액션 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	/** 사격 입력 액션 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;

	/** 재장전 입력 액션 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;

	// ========================================
	// 이동 설정
	// ========================================

	/** 걷기 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	/** 점프 높이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpZVelocity;

	// ========================================
	// 카메라 설정
	// ========================================

	/** 카메라 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraDistance;

	/** 카메라 위치 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector CameraLocationOffset;

	/** 마우스 좌우 감도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float LookSensitivityX;

	/** 마우스 상하 감도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float LookSensitivityY;

	/** 카메라 각도 제한 (아래) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float PitchMin;

	/** 카메라 각도 제한 (위) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float PitchMax;

	// ========================================
	// 총기 시스템
	// ========================================

	/** 총기 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	/** 현재 보유 중인 총기 참조 */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	AWeaponBase* CurrentWeapon;

	// ========================================
// 특수 탄환 시스템
// ========================================

/** 특수 탄환 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Weapon")
	TSubclassOf<AProjectile> SpecialProjectileClass;

	/** 현재 특수 탄환 개수 */
	UPROPERTY(BlueprintReadOnly, Category = "Special Weapon")
	int32 SpecialAmmo = 0;

	/** 최대 특수 탄환 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Weapon")
	int32 MaxSpecialAmmo = 3;

	/** 특수 탄환 충전 시간 (10초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Weapon")
	float SpecialAmmoRechargeTime = 10.0f;

	/** 특수 탄환 충전 타이머 */
	FTimerHandle SpecialAmmoRechargeHandle;

	/** 특수 탄환 발사 */
	UFUNCTION(BlueprintCallable, Category = "Special Weapon")
	void FireSpecialProjectile();

	/** 특수 탄환 충전 시작 */
	UFUNCTION()
	void StartSpecialAmmoRecharge();

	/** 특수 탄환 충전 완료 */
	UFUNCTION()
	void OnSpecialAmmoRecharged();

	// ========================================
	// Q 스킬 - 기폭
	// ========================================

	/** Q 스킬 쿨다운 시간 (50초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float QSkillCooldown = 50.0f;

	/** Q 스킬 사용 가능 여부 */
	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	bool bCanUseQSkill = true;

	/** Q 스킬 쿨다운 타이머 */
	FTimerHandle QSkillCooldownHandle;

	/** Q 스킬 실행 */
	UFUNCTION(BlueprintCallable, Category = "Skills")
	void UseQSkill();

	/** Q 스킬 쿨다운 완료 */
	UFUNCTION()
	void OnQSkillCooldownFinished();

	/** 기폭 범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float DetonationRadius = 5000.0f;

	/** 기폭 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float DetonationDamage = 100.0f;

	/** Q 스킬 기폭 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Effects")
	UParticleSystem* QSkillExplosionEffect;


	// ========================================
	// E 스킬 - 대쉬
	// ========================================

	/** E 스킬 쿨다운 시간 (5초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float ESkillCooldown = 5.0f;

	/** E 스킬 사용 가능 여부 */
	UPROPERTY(BlueprintReadOnly, Category = "Skills")
	bool bCanUseESkill = true;

	/** E 스킬 쿨다운 타이머 */
	FTimerHandle ESkillCooldownHandle;

	/** E 스킬 실행 (대쉬) */
	UFUNCTION(BlueprintCallable, Category = "Skills")
	void UseESkill();

	/** E 스킬 쿨다운 완료 */
	UFUNCTION()
	void OnESkillCooldownFinished();

	/** 대쉬 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float DashDistance = 1000.0f;

	/** 대쉬 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float DashSpeed = 5000.0f;

	// HyperFPSCharacter.h의 protected 섹션에 추가

	/** E 스킬 대쉬 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Effects")
	UParticleSystem* ESkillDashEffect;
	

/** 특수 탄환 개수 반환 */
	UFUNCTION(BlueprintPure, Category = "UI")
	int32 GetSpecialAmmo() const { return SpecialAmmo; }

	/** 특수 탄환 최대 개수 반환 */
	UFUNCTION(BlueprintPure, Category = "UI")
	int32 GetMaxSpecialAmmo() const { return MaxSpecialAmmo; }

	// HyperFPSCharacter.h의 protected 섹션에 추가

/** 특수 탄환 충전 진행도 (0.0 ~ 1.0) */
	UFUNCTION(BlueprintPure, Category = "UI")
	float GetSpecialAmmoRechargePercent() const;

	// HyperFPSCharacter.h의 protected 섹션에 추가

/** Q 스킬 사용 가능 여부 */
	UFUNCTION(BlueprintPure, Category = "UI")
	bool CanUseQSkill() const { return bCanUseQSkill; }

	/** Q 스킬 쿨다운 진행도 (0.0 ~ 1.0, 0 = 사용 가능) */
	UFUNCTION(BlueprintPure, Category = "UI")
	float GetQSkillCooldownPercent() const;

	// HyperFPSCharacter.h의 protected 섹션에 추가

/** E 스킬 사용 가능 여부 */
	UFUNCTION(BlueprintPure, Category = "UI")
	bool CanUseESkill() const { return bCanUseESkill; }

	/** E 스킬 쿨다운 진행도 (0.0 ~ 1.0, 0 = 사용 가능) */
	UFUNCTION(BlueprintPure, Category = "UI")
	float GetESkillCooldownPercent() const;

protected:
	// ========================================
	// 입력 처리 함수
	// ========================================

	/** WASD 이동 처리 */
	void Move(const FInputActionValue& Value);

	/** 마우스 시점 회전 처리 */
	void Look(const FInputActionValue& Value);

	/** 점프 시작 */
	void StartJump();

	/** 점프 끝 */
	void StopJump();

	/** 사격 시작 */
	void StartFiring();

	/** 사격 종료 */
	void StopFiring();

	/** 재장전 */
	void Reload();

public:
	// Getter 함수들
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};