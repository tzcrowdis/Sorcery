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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float DamageResistancePercent;

	/* Elemental Weakness */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elements", meta = (AllowPrivateAccess = "true"))
	EElementalType ElementWeakness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elements", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ElementalWeaknessMesh;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Fire;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Shock;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Acid;

	UPROPERTY(EditAnywhere, Category = "Elements")
	UMaterialInterface* M_Dark;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UMaterialInterface* M_Critical;

public:
	// Sets default values for this character's properties
	AEnemy();

	/* AI */
	class AEnemyController* EnemyController; // NOTE unique to variant???
	
	UPROPERTY(EditAnywhere, Category = "Behavior")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = "Behavior")
	class USphereComponent* AttackSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bInAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	/* Weak Spot */
	UPROPERTY(VisibleDefaultsOnly, Category = "Combat")
	class USphereComponent* WeakSpotComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float WeakSpotMultiplier;

	/* Floating Damage Variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageText")
	float DamageTaken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageText")
	bool bWeakSpotHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageText")
	FVector ImpactPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
	int SoulsValue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float GetDamageResistance(UDamageType* DamageType);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void RandomizeElementalWeakness();

	UFUNCTION() // NOTE should be overriden
	virtual void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION() // NOTE should be overriden
	virtual void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float TakeDamage
	(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	virtual void Die(AActor* DeathCauser);

	// Called to update damage text
	UFUNCTION(BlueprintImplementableEvent, Category = "DamageText")
	void DrawFloatingDamageText();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void ApplyDamageFlash(const UDamageType* ElementType);

	UFUNCTION(BlueprintCallable)
	UMaterialInterface* GetDamageFlashMaterial(UDamageType* ElementType, bool CriticalHit);
};
