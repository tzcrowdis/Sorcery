// Copyright Epic Games, Inc. All Rights Reserved.

#include "SorceryProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "DT_Fire.h"
#include "DT_Ice.h"
#include "DT_Acid.h"
#include "DT_Shock.h"

ASorceryProjectile::ASorceryProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASorceryProjectile::OnHit);		// set up a notification for when this component hits something blocking

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

void ASorceryProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // NOTE assumes single player

			if (OtherComp == Enemy->WeakSpotComp)
				UGameplayStatics::ApplyDamage(Enemy, Damage * Enemy->WeakSpotMultiplier, PlayerController, this, GetDamageType());
			else
				UGameplayStatics::ApplyDamage(Enemy, Damage, PlayerController, this, GetDamageType());
				
			Destroy();
		}
		
		// Only add impulse and destroy projectile if we hit a physics
		if ((OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
			Destroy();
		}	
	}
}

void ASorceryProjectile::ChangeElementalType(EElementalType NewType)
{
	ProjectileElement = NewType;
	switch (ProjectileElement)
	{
		case EElementalType::Fire:
			SphereMesh->SetMaterial(0, M_Fire);
			break;
		case EElementalType::Ice:
			SphereMesh->SetMaterial(0, M_Ice);
			break;
		case EElementalType::Shock:
			SphereMesh->SetMaterial(0, M_Shock);
			break;
		case EElementalType::Acid:
			SphereMesh->SetMaterial(0, M_Acid);
			break;
	}
}

UClass* ASorceryProjectile::GetDamageType()
{
	switch (ProjectileElement)
	{
		case EElementalType::Fire:
			return UDT_Fire::StaticClass();
		case EElementalType::Ice:
			return UDT_Ice::StaticClass();
		case EElementalType::Shock:
			return UDT_Shock::StaticClass();
		case EElementalType::Acid:
			return UDT_Acid::StaticClass();
	}

	return nullptr;
}