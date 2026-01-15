// Fill out your copyright notice in the Description page of Project Settings.


#include "ChainLightning.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


AChainLightning::AChainLightning()
{
	LightningRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LightningRangeSphere"));
	SetRootComponent(LightningRangeSphere);

	Damage = 5.f;
	ChainQuantityBreadth = 1;
	ChainQuantityDepth = 0;
}

void AChainLightning::SetChainLightningParams(int32 ChainBreadth, int32 ChainDepth, float NewDamage)
{
	ChainQuantityDepth = ChainDepth;
	ChainQuantityBreadth = ChainBreadth;
	Damage = NewDamage;
}

void AChainLightning::ApplyChainLightning(AActor* AttachedEnemy, UClass* DamageType, FLinearColor ElementColor, AActor* PreviousEnemy)
{
	// get enemies in range and sort by distance, if necessary
	TArray<AActor*> DamageableActors;
	LightningRangeSphere->GetOverlappingActors(DamageableActors, AEnemy::StaticClass());

	DamageableActors.Remove(AttachedEnemy);
	if (PreviousEnemy != nullptr) DamageableActors.Remove(PreviousEnemy);

	if (DamageableActors.Num() == 0)
	{
		Destroy();
		return;
	}

	if (DamageableActors.Num() > ChainQuantityBreadth)
	{
		FVector ActorLocation = GetActorLocation();
		DamageableActors.Sort([ActorLocation](const AActor& A, const AActor& B)
			{
				float DistanceASquared = FVector::DistSquared(ActorLocation, A.GetActorLocation());
				float DistanceBSquared = FVector::DistSquared(ActorLocation, B.GetActorLocation());
				return DistanceASquared < DistanceBSquared;
			});
	}

	int Splits = ChainQuantityBreadth;
	if (DamageableActors.Num() < ChainQuantityBreadth)
		Splits = DamageableActors.Num();

	// apply the lightning effect and damage
	for (int i = 0; i < Splits; i++)
	{
		SpawnLightningEffect(DamageableActors[i]->GetActorLocation(), ElementColor);
		ApplyLightningDamage(DamageableActors[i], DamageType);
		PlayLightningSound();
		
		if (ChainQuantityDepth > 0)
			SpawnChildLightning(DamageableActors[i], DamageType, ElementColor, AttachedEnemy);
	}

	DelayedDestroy();
}

void AChainLightning::SpawnLightningEffect(FVector TargetPosition, FLinearColor ElementColor)
{
	UNiagaraComponent* LightningEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
		LightningParticleSystem,
		GetRootComponent(),
		NAME_None,
		FVector(0.f, 0.f, 50.f),
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true,
		true,
		ENCPoolMethod::None,
		true
	);
	LightningEffect->SetVectorParameter(FName("TargetPosition"), TargetPosition + FVector(0.f, 0.f, 50.f));
	LightningEffect->SetColorParameter(FName("LightningColor"), ElementColor);
}

void AChainLightning::ApplyLightningDamage(AActor* TargetEnemy, UClass* DamageType)
{
	AEnemy* Enemy = Cast<AEnemy>(TargetEnemy);
	if (Enemy)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // NOTE assumes single player

		Enemy->bWeakSpotHit = false;
		UGameplayStatics::ApplyDamage(Enemy, Damage, PlayerController, this, DamageType);
	}
}