// Copyright Epic Games, Inc. All Rights Reserved.

#include "SorceryProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "SorceryCharacter.h"

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "DT_Fire.h"
#include "DT_Ice.h"
#include "DT_Acid.h"
#include "DT_Shock.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
		case EElementalType::Ice:
			TrailEffect->SetColorParameter(FName("TrailColor"), IceColor);
			break;
		case EElementalType::Shock:
			TrailEffect->SetColorParameter(FName("TrailColor"), ShockColor);
			break;
		case EElementalType::Acid:
			TrailEffect->SetColorParameter(FName("TrailColor"), AcidColor);
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
			if (OtherComp == Enemy->AttackSphere) 
				return; // NOTE i gotta be misunderstanding a collision setting but this should work
			
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // NOTE assumes single player

			Enemy->ImpactPoint = Hit.Location + 10.f * Hit.Normal; // Hit.Normal so it isn't in the mesh
			if (OtherComp == Enemy->WeakSpotComp)
			{
				Enemy->bWeakSpotHit = true;
				UGameplayStatics::ApplyDamage(Enemy, Damage * Enemy->WeakSpotMultiplier, PlayerController, this, GetDamageType());
			}
			else
			{
				Enemy->bWeakSpotHit = false;
				UGameplayStatics::ApplyDamage(Enemy, Damage, PlayerController, this, GetDamageType());
			}
			
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
}

void ASorceryProjectile::ChangeElementalType(EElementalType NewType)
{
	Super::ChangeElementalType(NewType);
	
	switch (Element)
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
		case EElementalType::Ice:
			HitEffect->SetColorParameter(FName("HitColor"), IceColor);
			break;
		case EElementalType::Shock:
			HitEffect->SetColorParameter(FName("HitColor"), ShockColor);
			break;
		case EElementalType::Acid:
			HitEffect->SetColorParameter(FName("HitColor"), AcidColor);
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
		case EElementalType::Ice:
			DeathEffect->SetColorParameter(FName("DeathColor"), IceColor);
			break;
		case EElementalType::Shock:
			DeathEffect->SetColorParameter(FName("DeathColor"), ShockColor);
			break;
		case EElementalType::Acid:
			DeathEffect->SetColorParameter(FName("DeathColor"), AcidColor);
			break;
	}
}