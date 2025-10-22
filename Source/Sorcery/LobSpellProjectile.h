// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SorceryProjectile.h"
#include "LobSpellProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SORCERY_API ALobSpellProjectile : public ASorceryProjectile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Radial Damage")
	float InnerRadius;
	
	UPROPERTY(EditAnywhere, Category = "Radial Damage")
	float OuterRadius;

	UPROPERTY(EditAnywhere, Category = "Radial Damage")
	float DamageFalloffExponent;

	UPROPERTY(EditAnywhere, Category = "Radial Damage")
	float MinimumDamage;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	ALobSpellProjectile();

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

};
