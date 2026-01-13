// Copyright Epic Games, Inc. All Rights Reserved.

#include "SorceryProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "SorceryCharacter.h"

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "DT_Fire.h"
#include "DT_Shock.h"
#include "DT_Dark.h"
#include "DT_Acid.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASorceryProjectile::ASorceryProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASorceryProjectile::OnHit);

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

	// scale of particle effects
	HitEffectScale = FVector(0.5f, 0.5f, 0.5f);
	DeathEffectScale = FVector(0.5f, 0.5f, 0.5f);
	TrailEffectScale = FVector(0.5f, 0.5f, 0.5f);
}

void ASorceryProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// spawn trail effect
	if (SpellTrailEffect != nullptr)
	{
		UNiagaraComponent* TrailEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
			SpellTrailEffect,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			false,
			true,
			ENCPoolMethod::None,
			true
		);

		switch (Element)
		{
		case EElementalType::Fire:
			TrailEffect->SetColorParameter(FName("TrailColor"), FireColor);
			break;
		case EElementalType::Shock:
			TrailEffect->SetColorParameter(FName("TrailColor"), ShockColor);
			break;
		case EElementalType::Acid:
			TrailEffect->SetColorParameter(FName("TrailColor"), AcidColor);
			break;
		case EElementalType::Dark:
			TrailEffect->SetColorParameter(FName("TrailColor"), DarkColor);
			break;
		}
	}
}

void ASorceryProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		ASorceryCharacter* Player = Cast<ASorceryCharacter>(OtherActor);
		if (Player)
			return;
		
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			ApplyDamageToEnemy(Enemy, OtherComp, Damage, this, GetDamageType());
			Destroy();
		}
		
		// Only add impulse and destroy projectile if we hit a physics
		if ((OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
			Destroy();
		}

		if (!Enemy)
			SpawnHitEffect(Hit.Normal);
	}

	PlayProjectileHitSound();
}

void ASorceryProjectile::ChangeElementalType(EElementalType NewType)
{
	Super::ChangeElementalType(NewType);
	
	switch (Element)
	{
		case EElementalType::Fire:
			SphereMesh->SetMaterial(0, M_Fire);
			break;
		case EElementalType::Shock:
			SphereMesh->SetMaterial(0, M_Shock);
			break;
		case EElementalType::Acid:
			SphereMesh->SetMaterial(0, M_Acid);
			break;
		case EElementalType::Dark:
			SphereMesh->SetMaterial(0, M_Dark);
			break;
	}
}

void ASorceryProjectile::SpawnHitEffect(FVector Normal)
{
	if (SpellHitEffect == nullptr)
		return;

	UNiagaraComponent* HitEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		SpellHitEffect,
		GetActorLocation(),
		UKismetMathLibrary::Conv_VectorToRotator(Normal),
		HitEffectScale
	);
	
	switch (Element)
	{
		case EElementalType::Fire:
			HitEffect->SetColorParameter(FName("HitColor"), FireColor);
			break;
		case EElementalType::Shock:
			HitEffect->SetColorParameter(FName("HitColor"), ShockColor);
			break;
		case EElementalType::Acid:
			HitEffect->SetColorParameter(FName("HitColor"), AcidColor);
			break;
		case EElementalType::Dark:
			HitEffect->SetColorParameter(FName("HitColor"), DarkColor);
			break;
	}
}

void ASorceryProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SpellDeathEffect == nullptr)
		return;

	UNiagaraComponent* DeathEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		SpellDeathEffect,
		GetActorLocation(),
		FRotator(0, 0, 0),
		DeathEffectScale
	);

	switch (Element)
	{
		case EElementalType::Fire:
			DeathEffect->SetColorParameter(FName("DeathColor"), FireColor);
			break;
		case EElementalType::Shock:
			DeathEffect->SetColorParameter(FName("DeathColor"), ShockColor);
			break;
		case EElementalType::Acid:
			DeathEffect->SetColorParameter(FName("DeathColor"), AcidColor);
			break;
		case EElementalType::Dark:
			DeathEffect->SetColorParameter(FName("DeathColor"), DarkColor);
			break;
	}

	PlayProjectileDeathSound();
}