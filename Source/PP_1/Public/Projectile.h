#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class PP_1_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	// ========================================
	// 컴포넌트
	// ========================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	// ========================================
	// 설정
	// ========================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ProjectileSpeed = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Lifetime = 5.0f;

	/** 트레일 파티클 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* TrailParticle;

	/** 트레일 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* TrailEffect;

	// ========================================
	// 이펐트
	// ========================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* ImpactEffect;


	// ========================================
	// 타이머
	// ========================================
	FTimerHandle LifetimeHandle;

	// ========================================
	// 충돌 공통 처리
	// ========================================
	virtual void HandleHit(AActor* OtherActor, const FHitResult& Hit);

	// ========================================
	// Hit 이벤트 (바닥, 벽과의 충돌)
	// ========================================
	UFUNCTION()                                                          
	void OnSphereHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherPrimitiveComponent,
		FVector NormalImpulse,
		const FHitResult& Hit);

	// ========================================
	// Overlap 이벤트 (캐릭터와의 충돌)
	// ========================================
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherPrimitiveComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};