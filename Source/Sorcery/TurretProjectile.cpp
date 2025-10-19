// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SorceryCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TurretEnemy.h"

// Sets default values
ATurretProjectile::ATurretProjectile()
{
	Damage = 5.f;
	
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ATurretProjectile::OnHit);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Projectile Mesh
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	SphereMesh->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ATurretProjectile::SetEnemyOwner(ATurretEnemy* NewOwner)
{
	Owner = NewOwner;
}

void ATurretProjectile::SetProjectileHomingTarget(USceneComponent* Target)
{
	if (Target)
		ProjectileMovement->HomingTargetComponent = Target;
}

void ATurretProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		ASorceryCharacter* Player = Cast<ASorceryCharacter>(OtherActor);
		if (Player && Owner != nullptr)
		{
			UGameplayStatics::ApplyDamage(Player, Damage, Owner->GetController(), this, UDamageType::StaticClass());
		}
	}

	Destroy();
}

void ATurretProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ExplodeEffect == nullptr)
		return;

	UNiagaraComponent* DeathEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		ExplodeEffect,
		GetActorLocation(),
		FRotator(0, 0, 0),
		FVector(0.5f, 0.5f, 0.5f)
	);
}