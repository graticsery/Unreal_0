#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SpecialProjectile.generated.h"

/**
 * 특수 탄환
 * 적중 시 타겟에 표식을 남기고, Q 스킬로 기폭 가능
 */
UCLASS()
class PP_1_API ASpecialProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	ASpecialProjectile();

protected:
	virtual void BeginPlay() override;

	// ========================================
	// 충돌 오버라이드
	// ========================================
	
	virtual void HandleHit(AActor* OtherActor, const FHitResult& Hit) override;

	// ========================================
	// 표식 시스템
	// ========================================
	
	/** 표식 이펙트 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark")
	TSubclassOf<AActor> MarkActorClass;

	/** 표식 Widget 클래스 */  // ← 이 줄부터 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark")
	TSubclassOf<UUserWidget> MarkWidgetClass;  // ← 여기까지 추가

	/** 표식 지속 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mark")
	float MarkDuration = 10.0f;
};
