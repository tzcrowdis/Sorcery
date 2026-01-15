// Fill out your copyright notice in the Description page of Project Settings.


#include "StarEnemy.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SorceryCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"


AStarEnemy::AStarEnemy()
{
	ElementalWeaknessMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElementalWeaknessMesh"));
	ElementalWeaknessMesh->SetupAttachment(WeakSpotComp);

	RecoveryPosition = CreateDefaultSubobject<USphereComponent>(TEXT("RecoveryPosition"));
	RecoveryPosition->OnComponentBeginOverlap.AddDynamic(this, &AStarEnemy::RecoveryPositionBeginOverlap);

	GetCharacterMovement()->Deactivate(); // want this enemy to act as a projectile

	AttackSpeed = 2000.f;
	RecoverSpeed = 1000.f;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = RecoverSpeed;
	ProjectileMovement->MaxSpeed = RecoverSpeed;
	ProjectileMovement->bRotationFollowsVelocity = false;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AStarEnemy::OnHit);

	Damage = 5.f;
}

void AStarEnemy::BeginPlay()
{
	Super::BeginPlay();

	// NOTE multiplayer problem
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Sorcerer)
		LastPlayerTargeted = Sorcerer; // otherwise they arent a threat until the player engages
	
	EnterRecoverState();
}

void AStarEnemy::RandomizeElementalWeakness()
{
	int32 randomElement = FMath::RandRange(0, 3);
	switch (static_cast<EElementalType>(randomElement))
	{
	case EElementalType::Fire:
		ElementWeakness = EElementalType::Fire;
		ElementalWeaknessMesh->SetMaterial(0, M_Fire);
		break;
	case EElementalType::Shock:
		ElementWeakness = EElementalType::Shock;
		ElementalWeaknessMesh->SetMaterial(0, M_Shock);
		break;
	case EElementalType::Acid:
		ElementWeakness = EElementalType::Acid;
		ElementalWeaknessMesh->SetMaterial(0, M_Acid);
		break;
	case EElementalType::Dark:
		ElementWeakness = EElementalType::Dark;
		ElementalWeaknessMesh->SetMaterial(0, M_Dark);
		break;
	}
}

void AStarEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag(TEXT("Projectile")))
		return;
	
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
	if (Sorcerer && bAttacking)
	{
		UGameplayStatics::ApplyDamage(Sorcerer, Damage, GetController(), this, UDamageType::StaticClass());
	}

	EnterRecoverState();
}

void AStarEnemy::AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
	if (Sorcerer)
	{
		PlayerTarget = Sorcerer;
		LastPlayerTargeted = Sorcerer;
	}
}

void AStarEnemy::AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
	if (Sorcerer)
	{
		PlayerTarget = nullptr;
	}
}

void AStarEnemy::EnterAttackState()
{
	bAttacking = true;
	
	WeakSpotComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	ProjectileMovement->MaxSpeed = AttackSpeed;
	ProjectileMovement->HomingTargetComponent = PlayerTarget->GetRootComponent();

	RecoveryPosition->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AStarEnemy::EnterRecoverState()
{
	bAttacking = false;
	
	WeakSpotComp->SetRelativeLocation(FVector(50.f, 0.f, 0.f));

	ProjectileMovement->MaxSpeed = RecoverSpeed;
	SetRecoverPosition();
	ProjectileMovement->HomingTargetComponent = RecoveryPosition;

	RecoveryPosition->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AStarEnemy::SetRecoverPosition()
{
	FNavLocation RecoveryLocation;
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	
	NavSystem->GetRandomReachablePointInRadius(
		(LastPlayerTargeted != nullptr) ? LastPlayerTargeted->GetActorLocation() : GetActorLocation(),
		RecoveryRadius,
		RecoveryLocation
	);
	RecoveryLocation.Location.Z += RecoveryHeightOffset + FMath::FRandRange(-RecoveryHeightVariance, RecoveryHeightVariance);

	RecoveryPosition->SetWorldLocation(RecoveryLocation.Location);
}

void AStarEnemy::RecoveryPositionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor == this)
	{
		if (PlayerTarget != nullptr)
		{
			EnterAttackState();
			return;
		}
	}

	SetRecoverPosition();
}

void AStarEnemy::StartAttackCooldown()
{
	GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AStarEnemy::ClearAttackCooldown, AttackCooldownTime);
	AttackCooldownActive = true;
}

void AStarEnemy::ClearAttackCooldown()
{
	GetWorldTimerManager().ClearTimer(AttackCooldownTimer);
	AttackCooldownActive = false;
}