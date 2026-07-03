#include "SpecialProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"  // ← 추가
#include "TargetDummy.h"

// 생성자
ASpecialProjectile::ASpecialProjectile()
{
	// 특수 탄환은 일반 탄환보다 느림
	ProjectileSpeed = 2000.0f;

	// 데미지 동일
	Damage = 25.0f;
}

// 게임 시작 시 호출
void ASpecialProjectile::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("SpecialProjectile spawned!"));
}

// ========================================
// 충돌 처리 (표식 추가)
// ========================================

void ASpecialProjectile::HandleHit(AActor* OtherActor, const FHitResult& Hit)
{
    UE_LOG(LogTemp, Warning, TEXT("SpecialProjectile HandleHit called!"));

    Super::HandleHit(OtherActor, Hit);

    if (OtherActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *OtherActor->GetName());

        // ========================================
        // 타겟 더미인지 확인
        // ========================================
        ATargetDummy* TargetDummy = Cast<ATargetDummy>(OtherActor);
        if (!TargetDummy)
        {
            UE_LOG(LogTemp, Warning, TEXT("Not a TargetDummy, skip marking"));
            return;  // 타겟 더미가 아니면 표식 안 함
        }

        // ========================================
        // 표식 추가 (타겟 더미에만)
        // ========================================

        OtherActor->Tags.Add(FName("Marked"));

        int32 MarkCount = 0;
        for (const FName& Tag : OtherActor->Tags)
        {
            if (Tag == FName("Marked"))
            {
                MarkCount++;
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Added 'Marked' tag to: %s (Total marks: %d)"),
            *OtherActor->GetName(), MarkCount);

        // ========================================
        // 표식 Widget 생성
        // ========================================

        if (!MarkWidgetClass)
        {
            UE_LOG(LogTemp, Error, TEXT("MarkWidgetClass is NULL!"));
            return;
        }

        UE_LOG(LogTemp, Warning, TEXT("Creating mark widget..."));

        UUserWidget* MarkWidget = CreateWidget<UUserWidget>(GetWorld(), MarkWidgetClass);

        if (!MarkWidget)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create mark widget!"));
            return;
        }

        UE_LOG(LogTemp, Warning, TEXT("Mark widget created successfully!"));

        // WorldLocation 설정
        FVector WidgetWorldLocation = OtherActor->GetActorLocation() +
            FVector(0.0f, MarkCount * 60.0f, 100.0f);

        UE_LOG(LogTemp, Warning, TEXT("Widget World Location: %s"), *WidgetWorldLocation.ToString());

        // Reflection으로 WorldLocation 변수 설정
        FProperty* WorldLocationProp = MarkWidget->GetClass()->FindPropertyByName(FName("WorldLocation"));

        if (!WorldLocationProp)
        {
            UE_LOG(LogTemp, Error, TEXT("WorldLocation property not found!"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("WorldLocation property found!"));
            void* ValuePtr = WorldLocationProp->ContainerPtrToValuePtr<void>(MarkWidget);
            WorldLocationProp->CopyCompleteValue(ValuePtr, &WidgetWorldLocation);
            UE_LOG(LogTemp, Warning, TEXT("WorldLocation set!"));
        }

        // 화면에 추가
        MarkWidget->AddToViewport(100);
        UE_LOG(LogTemp, Warning, TEXT("Widget added to viewport!"));

        // 타겟에 Widget 저장
        TargetDummy->MarkWidgets.Add(MarkWidget);
        UE_LOG(LogTemp, Warning, TEXT("Widget saved to TargetDummy!"));

        // 10초 후 자동 삭제
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [MarkWidget]() {
                UE_LOG(LogTemp, Warning, TEXT("Removing mark widget..."));
                MarkWidget->RemoveFromParent();
            },
            MarkDuration,
            false
        );
    }
}