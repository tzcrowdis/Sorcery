// Fill out your copyright notice in the Description page of Project Settings.


#include "LobSpell.h"
#include "SorceryProjectile.h"
#include "Components/AudioComponent.h"

ALobSpell::ALobSpell()
{
	MuzzleFlashComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlashEffect"));
	RootComponent = MuzzleFlashComp;

	SpellSound = CreateDefaultSubobject<UAudioComponent>(TEXT("SpellSound"));
	SpellSound->SetupAttachment(RootComponent);

	bShootCooldownActive = false;
}


void ALobSpell::ChangeElementalType(EElementalType NewType)
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

/*
	SHOOTING
*/
void ALobSpell::ShootLobSpell(ASorceryCharacter* Sorcerer)
{
	if (Sorcerer->GetController() == nullptr)
		return;

	if (bShootCooldownActive)
		return;
	else
	{
		GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ALobSpell::ClearShootCooldown, AttackSpeed);
		bShootCooldownActive = true;
	}

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
			ASorceryProjectile* ball = World->SpawnActor<ASorceryProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if (ball)
			{
				ball->ChangeElementalType(Element);
				ball->UpdateDamage(Damage / BaseDamage);
			}

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

	// Try and play the sound
	SpellSound->Play();
}

void ALobSpell::ClearShootCooldown()
{
	GetWorldTimerManager().ClearTimer(ShootCooldownTimer);
	bShootCooldownActive = false;
}