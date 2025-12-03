// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretEnemy.h"
#include "TurretProjectile.h"
#include "EnemyController.h"
#include "SorceryCharacter.h"
#include "Components/SphereComponent.h"

ATurretEnemy::ATurretEnemy()
{
	bShootCooldownActive = false;
	ShootCooldownTime = 2.5f;

	VolleyIndex = 1;
	VolleyCount = 3;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

	WeakSpotComp->SetupAttachment(GetMesh(), FName(TEXT("T1_EndSocket")));

	WeakSpotComp1 = CreateDefaultSubobject<USphereComponent>(TEXT("WeakSpot1"));
	WeakSpotComp1->SetupAttachment(GetMesh(), FName(TEXT("T2_EndSocket")));

	WeakSpotComp2 = CreateDefaultSubobject<USphereComponent>(TEXT("WeakSpot2"));
	WeakSpotComp2->SetupAttachment(GetMesh(), FName(TEXT("T3_EndSocket")));
}

void ATurretEnemy::SpawnHomingProjectile()
{
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
			const FRotator SpawnRotation = ProjectileSpawnPoint->GetUpVector().Rotation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
			ATurretProjectile* Projectile = World->SpawnActor<ATurretProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			Projectile->SetProjectileHomingTarget(AttackTarget);
			Projectile->SetEnemyOwner(this);
		}
	}
}

void ATurretEnemy::StartShootCooldown()
{
	GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ATurretEnemy::ClearShootCooldown, ShootCooldownTime);
	bShootCooldownActive = true;
}

void ATurretEnemy::ClearShootCooldown()
{
	GetWorldTimerManager().ClearTimer(ShootCooldownTimer);
	bShootCooldownActive = false;
	VolleyIndex = 1;
}

void ATurretEnemy::AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			bInAttackRange = true;
			bAttacking = true;

			AttackTarget = Sorcerer->GetRootComponent();
		}
	}
}

void ATurretEnemy::AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			bInAttackRange = false;
			bAttacking = false;

			AttackTarget = nullptr;
		}
	}
}