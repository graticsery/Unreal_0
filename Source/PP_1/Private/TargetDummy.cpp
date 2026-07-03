#include "TargetDummy.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// 생성자
ATargetDummy::ATargetDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	// ========================================
	// Static Mesh 컴포넌트 생성
	// ========================================
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	// 충돌 설정
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
}

// 게임 시작 시 호출
void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();

	// 체력 초기화
	CurrentHealth = MaxHealth;
}

// 매 프레임 호출
void ATargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ========================================
// IDamageableInterface 구현
// ========================================

// 데미지 받기
void ATargetDummy::TakeDamageInterface_Implementation(float DamageAmount, AActor* DamageInstigator, AActor* DamageCauser)
{
	if (!IsAlive_Implementation()) return;

	// 체력 감소
	CurrentHealth -= DamageAmount;
	CurrentHealth = FMath::Max(0.0f, CurrentHealth);

	// 데미지 이펙트
	if (DamageEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DamageEffect,
			GetActorLocation()
		);
	}

	// 데미지 사운드
	if (DamageSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DamageSound,
			GetActorLocation()
		);
	}

	// ========================================
	// 데미지 인디케이터 위젯 생성
	// ========================================
	if (DamageIndicatorClass)
	{
		UUserWidget* DamageWidget = CreateWidget<UUserWidget>(GetWorld(), DamageIndicatorClass);
		if (DamageWidget)
		{
			// 디버그: DamageAmount 값 확인
			UE_LOG(LogTemp, Warning, TEXT("Setting DamageAmount: %f"), DamageAmount);

			// DamageAmount 변수 직접 설정
			if (FProperty* DamageAmountProp = DamageWidget->GetClass()->FindPropertyByName(FName("DamageAmount")))
			{
				UE_LOG(LogTemp, Warning, TEXT("Found DamageAmount property"));

				// Float 타입 시도
				if (FFloatProperty* FloatProp = CastField<FFloatProperty>(DamageAmountProp))
				{
					void* ValuePtr = FloatProp->ContainerPtrToValuePtr<void>(DamageWidget);
					FloatProp->SetFloatingPointPropertyValue(ValuePtr, DamageAmount);
					UE_LOG(LogTemp, Warning, TEXT("Set DamageAmount as Float successfully"));
				}
				// Double 타입 시도
				else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(DamageAmountProp))
				{
					void* ValuePtr = DoubleProp->ContainerPtrToValuePtr<void>(DamageWidget);
					DoubleProp->SetFloatingPointPropertyValue(ValuePtr, static_cast<double>(DamageAmount));
					UE_LOG(LogTemp, Warning, TEXT("Set DamageAmount as Double successfully"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("DamageAmount is neither Float nor Double property"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DamageAmount property not found"));
			}

			// WorldLocation 설정 (타겟 위쪽)
			FVector WidgetWorldLocation = GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
			UFunction* SetWorldLocationFunc = DamageWidget->FindFunction(FName("SetWorldLocation"));
			if (SetWorldLocationFunc)
			{
				struct FSetWorldLocationParams
				{
					FVector WorldLocation;
				};

				FSetWorldLocationParams LocationParams;
				LocationParams.WorldLocation = WidgetWorldLocation;

				DamageWidget->ProcessEvent(SetWorldLocationFunc, &LocationParams);
			}
			else
			{
				// 직접 변수 설정 (Reflection 사용)
				if (FProperty* WorldLocationProp = DamageWidget->GetClass()->FindPropertyByName(FName("WorldLocation")))
				{
					void* ValuePtr = WorldLocationProp->ContainerPtrToValuePtr<void>(DamageWidget);
					WorldLocationProp->CopyCompleteValue(ValuePtr, &WidgetWorldLocation);
				}
			}

			// 화면에 추가
			DamageWidget->AddToViewport(999); // 최상단 레이어
		}
	}

	// 체력이 0 이하면 파괴
	if (CurrentHealth <= 0.0f)
	{
		OnTargetDestroyed();
	}
}

// 현재 체력 반환
float ATargetDummy::GetCurrentHealth_Implementation() const
{
	return CurrentHealth;
}

// 최대 체력 반환
float ATargetDummy::GetMaxHealth_Implementation() const
{
	return MaxHealth;
}

// 살아있는지 확인
bool ATargetDummy::IsAlive_Implementation() const
{
	return CurrentHealth > 0.0f;
}

// ========================================
// 타겟 파괴
// ========================================

void ATargetDummy::OnTargetDestroyed()
{
	// 파괴 이펙트
	if (DestroyEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DestroyEffect,
			GetActorLocation()
		);
	}

	// 파괴 사운드
	if (DestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DestroySound,
			GetActorLocation()
		);
	}

	// 액터 파괴
	Destroy();
}
float ATargetDummy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	// 부모 클래스 호출
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 인터페이스 함수로 전달
	TakeDamageInterface_Implementation(DamageAmount, EventInstigator ? EventInstigator->GetPawn() : nullptr, DamageCauser);

	return ActualDamage;
}