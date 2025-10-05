// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spell.h"
#include "Sorcery.h"
#include "Enemy.h"
#include "SorceryProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS(config=Game)
class ASorceryProjectile : public ASpell
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* SphereMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	/* Projectile Elemental Type */
	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Fire;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Ice;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Shock;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Acid;

	/* Hit Effect */
	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	UNiagaraSystem* SpellHitEffect;

	/* Blow Up Effect */
	UPROPERTY(EditAnywhere, Category = "Death Effect")
	UNiagaraSystem* SpellDeathEffect;

public:
	ASorceryProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	void SpawnHitEffect(FVector Normal);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/* Functions to Change Projectile Properties */
	virtual void ChangeElementalType(EElementalType NewType) override;
};

