#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

// 이 클래스는 수정하지 않습니다. 리플렉션 시스템에서 사용됩니다.
UINTERFACE(MinimalAPI, Blueprintable)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 데미지를 받을 수 있는 액터를 위한 인터페이스
 */
class PP_1_API IDamageableInterface
{
	GENERATED_BODY()

public:
	/**
	 * 데미지를 받습니다.
	 * @param DamageAmount 받을 데미지 양
	 * @param DamageInstigator 데미지를 가한 액터
	 * @param DamageCauser 데미지의 직접적인 원인 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void TakeDamageInterface(float DamageAmount, AActor* DamageInstigator, AActor* DamageCauser);

	/**
	 * 현재 체력을 반환
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	float GetCurrentHealth() const;

	/**
	 * 최대 체력을 반환
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	float GetMaxHealth() const;

	/**
	 * 살아있는지 확인
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool IsAlive() const;
};