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
			ApplyDamageToEnemy(Enemy, OtherComp, Damage, this, GetDamageType());
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