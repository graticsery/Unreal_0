// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HyperFPSCharacter.h"
#include "DamageableInterface.h"

// 생성자
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// ========================================
	// Collision Sphere 생성
	// ========================================
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(5.0f);
	CollisionSphere->SetCollisionProfileName(TEXT("NoCollision"));
	SetRootComponent(CollisionSphere);

	// ========================================
	// Projectile Movement 생성
	// ========================================
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticle"));
	TrailParticle->SetupAttachment(RootComponent);
	TrailParticle->bAutoActivate = false;
}

// 게임 시작 시 호출
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// ========================================
	// 충돌 설정
	// WorldStatic, WorldDynamic만 Block (바닥, 벽)
	// Pawn은 Overlap (캐릭터는 코드에서 직접 처리)
	// PhysicsBody는 Ignore (발사체끼리 무시)
	// ========================================
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionObjectType(ECC_PhysicsBody);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// ========================================
	// Hit 이벤트 바인딩 (바닥, 벽과의 충돌)
	// ========================================
	CollisionSphere->OnComponentHit.AddDynamic(this, &AProjectile::OnSphereHit);

	// ========================================
	// Overlap 이벤트 바인딩 (캐릭터와의 충돌)
	// ========================================
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnSphereOverlap);

	if (TrailEffect && TrailParticle)
	{
		TrailParticle->SetTemplate(TrailEffect);
		TrailParticle->Activate();
	}

	// ========================================
	// 수명 타이머
	// ========================================
	GetWorldTimerManager().SetTimer(
		LifetimeHandle,
		[this]() { Destroy(); },
		Lifetime,
		false
	);
}

// 매 프레임 호출
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ========================================
// 충돌 공통 처리
// ========================================
void AProjectile::HandleHit(AActor* OtherActor, const FHitResult& Hit)
{
	// 자기 자신 무시
	if (OtherActor == this) return;

	// 발사자 무시
	if (OtherActor == GetInstigator()) return;

	// 다른 발사체 무시
	if (Cast<AProjectile>(OtherActor)) return;

	// ========================================
	// 충돌 이펐트
	// ========================================
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactEffect,
			Hit.Location,
			Hit.Normal.Rotation()
		);
	}

	// ========================================
	// 데미지 적용
	// ========================================
	if (OtherActor)
	{
		// 이펙트 스폰
		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactEffect,
				Hit.Location,
				Hit.Normal.Rotation()
			);
		}

		// IDamageableInterface 구현 여부 확인
		if (OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
		{
			// 인터페이스를 통해 데미지 적용
			IDamageableInterface::Execute_TakeDamageInterface(
				OtherActor,
				Damage,
				GetInstigator(),
				this
			);
		}
		else
		{
			// 인터페이스가 없으면 기존 방식 (ApplyDamage)
			APlayerController* InstigatorController = nullptr;
			if (GetInstigator())
			{
				InstigatorController = Cast<APlayerController>(GetInstigator()->GetController());
			}

			UGameplayStatics::ApplyDamage(
				OtherActor,
				Damage,
				InstigatorController,
				this,
				nullptr
			);
		}
	}

	// ========================================
	// 발사체 삭제
	// ========================================
	Destroy();
}

// ========================================
// Hit 이벤트 (바닥, 벽과의 충돌)
// ========================================
void AProjectile::OnSphereHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherPrimitiveComponent,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	HandleHit(OtherActor, Hit);
}

// ========================================
// Overlap 이벤트 (캐릭터와의 충돌)
// ========================================
void AProjectile::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherPrimitiveComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	HandleHit(OtherActor, SweepResult);
}