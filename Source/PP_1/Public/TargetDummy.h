#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageableInterface.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"
#include "TargetDummy.generated.h"


class UStaticMeshComponent;

/**
 * 허수아비 타겟
 * 데미지를 받으면 체력이 감소하고, 체력이 0이 되면 파괴됩니다.
 */
UCLASS()
class PP_1_API ATargetDummy : public AActor, public IDamageableInterface
{
	GENERATED_BODY()

public:
	ATargetDummy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	// AActor의 TakeDamage 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	// ========================================
	// 컴포넌트
	// ========================================

	/** 허수아비 메쉬 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// ========================================
	// 체력 시스템
	// ========================================

	/** 최대 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 1000.0f;

	/** 현재 체력 */
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 1000.0f;

	// ========================================
	// IDamageableInterface 구현
	// ========================================

	virtual void TakeDamageInterface_Implementation(float DamageAmount, AActor* DamageInstigator, AActor* DamageCauser) override;
	virtual float GetCurrentHealth_Implementation() const override;
	virtual float GetMaxHealth_Implementation() const override;
	virtual bool IsAlive_Implementation() const override;

	// ========================================
	// 이펙트
	// ========================================

	/** 데미지 받을 때 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* DamageEffect;

	/** 파괴될 때 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* DestroyEffect;

	/** 데미지 받을 때 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* DamageSound;

	/** 파괴될 때 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* DestroySound;

	// ========================================
	// UI
	// ========================================

	/** 데미지 인디케이터 위젯 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> DamageIndicatorClass;

	// TargetDummy.h에 추가

/** 표식 위젯 리스트 */
	UPROPERTY()
	TArray<UUserWidget*> MarkWidgets;

protected:
	/** 타겟이 파괴될 때 호출 */
	UFUNCTION()
	void OnTargetDestroyed();
};