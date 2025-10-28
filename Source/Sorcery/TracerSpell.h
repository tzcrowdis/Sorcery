// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "SorceryCharacter.h"
#include "TracerSpell.generated.h"

/**
 * 
 */
UCLASS()
class SORCERY_API ATracerSpell : public ASpell
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Aim")
	float MaxAimDistance;

	UPROPERTY(EditAnywhere, Category = "Aim")
	float ConeHalfAngleInDegrees;

	/* Muzzle flash */
	UPROPERTY(EditAnywhere, Category = "Muzzle Flash")
	UNiagaraSystem* SpellMuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "Muzzle Flash")
	UNiagaraComponent* MuzzleFlashComp;

protected:
	// player camera for trace
	class UCameraComponent* Camera;

	bool bShootCooldownActive;
	FTimerHandle ShootCooldownTimer;
	void ClearShootCooldown();

	virtual void BeginPlay() override;

public:
	ATracerSpell();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell")
	bool bTracerFiring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell")
	USceneComponent* AttackTarget;

	/** Default Projectile classes to spawn for each element */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASorceryProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void ShootTracerSpell(ASorceryCharacter* Sorcerer);

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void SpawnTracer();

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void DeactivateTracerSpell();

	virtual void ChangeElementalType(EElementalType NewType) override;
};
