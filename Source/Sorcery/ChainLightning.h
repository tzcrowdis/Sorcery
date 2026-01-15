// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellModifier.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "ChainLightning.generated.h"

class USphereComponent;

/**
 * 
 */
UCLASS()
class SORCERY_API AChainLightning : public ASpellModifier
{
	GENERATED_BODY()

	/* Stats */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 ChainQuantityBreadth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 ChainQuantityDepth;

	/* Lightning Effect */
	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* LightningParticleSystem;

	/* Collision */
	UPROPERTY(EditAnywhere, Category = "Combat")
	USphereComponent* LightningRangeSphere;

protected:
	void SpawnLightningEffect(FVector TargetPosition, FLinearColor ElementColor);

	void ApplyLightningDamage(AActor* TargetEnemy, UClass* DamageType);

public:
	AChainLightning();

	UFUNCTION(BlueprintCallable)
	void SetChainLightningParams(int32 ChainBreadth, int32 ChainDepth, float NewDamage);

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnChildLightning(AActor* AttachToActor, UClass* DamageType, FLinearColor ElementColor, AActor* PreviousActor);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyChainLightning(AActor* AttachedEnemy, UClass* DamageType, FLinearColor ElementColor, AActor* PreviousEnemy = nullptr);

	UFUNCTION(BlueprintImplementableEvent)
	void DelayedDestroy();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayLightningSound();

};
