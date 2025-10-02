// Fill out your copyright notice in the Description page of Project Settings.


#include "ElementalBallSpell.h"
#include "SorceryProjectile.h"

/*
	ADMIN
*/
AElementalBallSpell::AElementalBallSpell()
{
	MuzzleFlashComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlashEffect"));
	RootComponent = MuzzleFlashComp;
}

void AElementalBallSpell::BeginPlay()
{
	Super::BeginPlay();

}

void AElementalBallSpell::ChangeElementalType(EElementalType NewType)
{
	Super::ChangeElementalType(NewType);

	ASorceryProjectile* projectile = Cast<ASorceryProjectile>(ProjectileClass->GetDefaultObject());
	if (projectile) projectile->ChangeElementalType(Element);

	switch (Element)
	{
	case EElementalType::Fire:
		MuzzleFlashComp->SetAsset(FireSpellMuzzleFlash);
		break;
	case EElementalType::Ice:
		MuzzleFlashComp->SetAsset(IceSpellMuzzleFlash);
		break;
	case EElementalType::Shock:
		MuzzleFlashComp->SetAsset(ShockSpellMuzzleFlash);
		break;
	case EElementalType::Acid:
		MuzzleFlashComp->SetAsset(AcidSpellMuzzleFlash);
		break;
	}
	MuzzleFlashComp->Deactivate();
}

/*
	SHOOTING
*/
void AElementalBallSpell::ShootElementalBall(ASorceryCharacter* Sorcerer)
{
	if (Sorcerer->GetController() == nullptr)
		return;

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// Spawn location
			const FVector SpawnLocation = MuzzleFlashComp->GetComponentLocation();

			// aim point
			FVector AimPoint = GetAimLocation(Sorcerer->GetFirstPersonCameraComponent(), MaxAimDistance);

			// Spawn rotation
			FVector Direction = AimPoint - SpawnLocation;
			Direction.Normalize();
			const FRotator SpawnRotation = Direction.Rotation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<ASorceryProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			// muzzle flash
			if (MuzzleFlashComp)
			{
				if (MuzzleFlashComp->IsActive())
					MuzzleFlashComp->Deactivate();

				MuzzleFlashComp->SetWorldRotation(SpawnRotation);
				MuzzleFlashComp->Activate();
			}
		}
	}

	/*
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	*/
}