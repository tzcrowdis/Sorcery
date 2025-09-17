// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Sorcery.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class SORCERY_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float DamageResistancePercent;

	/* Elemental Weakness */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elements", meta = (AllowPrivateAccess = "true"))
	EElementalType ElementWeakness;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UStaticMeshComponent* ElementalWeaknessMesh;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Fire;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Ice;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Shock;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Acid;

public:
	// Sets default values for this character's properties
	AEnemy();

	/* AI */
	class AEnemyController* EnemyController;
	
	UPROPERTY(EditAnywhere, Category = "Behavior")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = "Behavior")
	class USphereComponent* AttackSphere;

	UPROPERTY(BlueprintReadOnly)
	bool bIsChasing;

	bool bInAttackRange;
	bool bAttacking;

	/* Weak Spot */
	UPROPERTY(VisibleDefaultsOnly, Category = "Combat")
	class USphereComponent* WeakSpotComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float WeakSpotMultiplier;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float GetDamageResistance(UDamageType* DamageType);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void RandomizeElementalWeakness();

	UFUNCTION()
	void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void IsChasing(bool bChasing);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float TakeDamage
	(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	virtual void Die(AActor* DeathCauser);
};
