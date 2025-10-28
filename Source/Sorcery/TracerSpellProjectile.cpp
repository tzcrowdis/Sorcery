// Fill out your copyright notice in the Description page of Project Settings.


#include "TracerSpellProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SorceryCharacter.h"
#include "Enemy.h"


// TODO spawn trail effect and change color with element


void ATracerSpellProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
				return;

			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // NOTE assumes single player

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
		}
	}

	Destroy();
}

void ATracerSpellProjectile::SetHomingTarget(USceneComponent* Target)
{
	if (Target)
		ProjectileMovement->HomingTargetComponent = Target;
	else
		ProjectileMovement->bIsHomingProjectile = false;
}