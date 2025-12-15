// Fill out your copyright notice in the Description page of Project Settings.


#include "LobSpellProjectile.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SorceryCharacter.h"
#include "Enemy.h"
#include "NiagaraFunctionLibrary.h"


ALobSpellProjectile::ALobSpellProjectile()
{
	InnerRadius = 150.f;
	OuterRadius = 300.f;
	DamageFalloffExponent = 0.5f;
	MinimumDamage = 5.f;
}

void ALobSpellProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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


void ALobSpellProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

	TArray<AActor*> ActorsToIgnore = { UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) };
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		Damage,
		GetActorLocation(),
		OuterRadius,
		GetDamageType(),
		ActorsToIgnore,
		this,
		nullptr,
		true,
		ECollisionChannel::ECC_Visibility
	);

	// falloff requires processing on the damage receivers part
	/*
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		Damage,
		MinimumDamage,
		GetActorLocation(),
		InnerRadius,
		OuterRadius,
		DamageFalloffExponent,
		GetDamageType(),
		ActorsToIgnore,
		this,
		nullptr,
		ECollisionChannel::ECC_Visibility
	);
	*/

	PlayProjectileDeathSound();
}
