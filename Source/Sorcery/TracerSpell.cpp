// Fill out your copyright notice in the Description page of Project Settings.


#include "TracerSpell.h"
#include "TracerSpellProjectile.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"


ATracerSpell::ATracerSpell()
{
	MuzzleFlashComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlashEffect"));
	RootComponent = MuzzleFlashComp;
	
	bShootCooldownActive = false;
	bTracerFiring = false;

	ConeHalfAngleInDegrees = 10.f;
}

void ATracerSpell::BeginPlay()
{
	Super::BeginPlay();

}

void ATracerSpell::ShootTracerSpell(ASorceryCharacter* Sorcerer)
{
	bTracerFiring = true;
	Camera = Sorcerer->GetFirstPersonCameraComponent();
	SpawnTracer();
}

void ATracerSpell::SpawnTracer()
{
	if (bShootCooldownActive)
		return;
	
	// check what the player is aiming at
	FHitResult HitResult = GetAimHitResult(Camera, MaxAimDistance);

	// spawn homing projectile
	const FVector SpawnLocation = MuzzleFlashComp->GetComponentLocation();
	
	// add randomness along cone
	FVector AimPoint = GetAimLocation(Camera, MaxAimDistance); // NOTE two hit scans this frame
	FVector ConeDirection = AimPoint - SpawnLocation;
	ConeDirection.Normalize();
	FVector RandomVectorInCone = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(
		ConeDirection,
		ConeHalfAngleInDegrees
	);
	const FRotator SpawnRotation = RandomVectorInCone.Rotation();

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ATracerSpellProjectile* Projectile = GetWorld()->SpawnActor<ATracerSpellProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	if (Projectile)
	{
		Projectile->ChangeElementalType(Element);
		Projectile->UpdateDamage(Damage / BaseDamage);

		// set homing target to enemy or null
		AttackTarget = nullptr;
		AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy) AttackTarget = HitResult.GetComponent();
		Projectile->SetHomingTarget(AttackTarget);
	}

	// start cooldown timer
	GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ATracerSpell::ClearShootCooldown, AttackSpeed);
	bShootCooldownActive = true;

	// muzzle flash
	if (MuzzleFlashComp)
	{
		if (MuzzleFlashComp->IsActive())
			MuzzleFlashComp->Deactivate();

		MuzzleFlashComp->SetWorldRotation(ConeDirection.Rotation());
		MuzzleFlashComp->Activate();
	}
}

void ATracerSpell::ClearShootCooldown()
{
	GetWorldTimerManager().ClearTimer(ShootCooldownTimer);
	bShootCooldownActive = false;

	if (bTracerFiring)
		SpawnTracer();
}

void ATracerSpell::DeactivateTracerSpell()
{
	bTracerFiring = false;
}

void ATracerSpell::ChangeElementalType(EElementalType NewType)
{
	Super::ChangeElementalType(NewType);

	switch (Element)
	{
		case EElementalType::Fire:
			MuzzleFlashComp->SetColorParameter(FName("FlashColor"), FireColor);
			break;
		case EElementalType::Shock:
			MuzzleFlashComp->SetColorParameter(FName("FlashColor"), ShockColor);
			break;
		case EElementalType::Acid:
			MuzzleFlashComp->SetColorParameter(FName("FlashColor"), AcidColor);
			break;
		case EElementalType::Dark:
			MuzzleFlashComp->SetColorParameter(FName("FlashColor"), DarkColor);
			break;
	}
	MuzzleFlashComp->Deactivate();
}