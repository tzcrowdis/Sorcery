// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "NiagaraComponent.h"
#include "Math/Color.h"
#include "LaserSpell.generated.h"

/**
 * 
 */
UCLASS(config=Game)
class SORCERY_API ALaserSpell : public ASpell
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Spells")
	UNiagaraComponent* LaserComp;

	UPROPERTY(EditAnywhere, Category = "Spells")
	float MaxLaserDistance;

	// color of origin orb
	UPROPERTY(EditAnywhere, Category = "Origin Material")
	UMaterialInterface* M_Fire;

	UPROPERTY(EditAnywhere, Category = "Origin Material")
	UMaterialInterface* M_Ice;

	UPROPERTY(EditAnywhere, Category = "Origin Material")
	UMaterialInterface* M_Shock;

	UPROPERTY(EditAnywhere, Category = "Origin Material")
	UMaterialInterface* M_Acid;

protected:
	virtual void BeginPlay() override;

	// player camera for trace
	class UCameraComponent* Camera;

public:
	ALaserSpell();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spells")
	bool bLaserFiring;

	UFUNCTION(BlueprintCallable, Category = "Spells")
	void ShootLaser(UCameraComponent* PlayerCamera);

	UFUNCTION(BlueprintCallable, Category = "Spells")
	void TickLaser();

	UFUNCTION(BlueprintCallable, Category = "Spells")
	void ApplyLaserDamage();

	UFUNCTION(BlueprintCallable, Category = "Spells")
	void DeactivateLaser();

	virtual void ChangeElementalType(EElementalType NewType) override;
};
