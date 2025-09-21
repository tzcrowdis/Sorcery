// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sorcery.h"
#include "Enemy.h"
#include "SorceryProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS(config=Game)
class ASorceryProjectile : public AActor
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	EElementalType ProjectileElement;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Fire;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Ice;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Shock;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Acid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float Damage;

	/* Hit Effect */
	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	UNiagaraSystem* FireHitEffect;

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	UNiagaraSystem* IceHitEffect;

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	UNiagaraSystem* ShockHitEffect;

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	UNiagaraSystem* AcidHitEffect;

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
	UClass* GetDamageType();

	void SpawnHitEffect(AEnemy* EnemyHit, FVector Normal);

public:
	/* Functions to Change Projectile Properties */
	void ChangeElementalType(EElementalType NewType);
};

