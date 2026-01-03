// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretEnemy.h"
#include "TurretProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class SORCERY_API ATurretProjectile : public AActor
{
	GENERATED_BODY()

	ATurretEnemy* Owner;

	UPROPERTY(EditAnywhere, Category = Projectile)
	float Damage;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* Mesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	/* Blow Up Effect */
	UPROPERTY(EditAnywhere, Category = "SFX")
	UNiagaraSystem* ExplodeEffect;
	
public:	
	// Sets default values for this actor's properties
	ATurretProjectile();

	UFUNCTION()
	void SetEnemyOwner(ATurretEnemy* NewOwner);

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	void SetProjectileHomingTarget(USceneComponent* Target);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
