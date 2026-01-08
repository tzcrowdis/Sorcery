// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderEnemy.h"
#include "Components/SphereComponent.h"
#include "SorceryCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"


ASpiderEnemy::ASpiderEnemy()
{
	ElementalWeaknessMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElementalWeaknessMesh"));
	ElementalWeaknessMesh->SetupAttachment(WeakSpotComp);

	Damage = 5.f;

	AttackCooldownActive = false;
	AttackCooldownTime = 2.f;
}

void ASpiderEnemy::BeginPlay()
{
	Super::BeginPlay();

	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Sorcerer)
		EnemyController->GetBlackboard()->SetValueAsObject(TEXT("TargetActor"), Sorcerer);
}

void ASpiderEnemy::RandomizeElementalWeakness()
{
	int32 randomElement = FMath::RandRange(0, 3);
	switch (static_cast<EElementalType>(randomElement))
	{
	case EElementalType::Fire:
		ElementWeakness = EElementalType::Fire;
		ElementalWeaknessMesh->SetMaterial(0, M_Fire);
		break;
	case EElementalType::Shock:
		ElementWeakness = EElementalType::Shock;
		ElementalWeaknessMesh->SetMaterial(0, M_Shock);
		break;
	case EElementalType::Acid:
		ElementWeakness = EElementalType::Acid;
		ElementalWeaknessMesh->SetMaterial(0, M_Acid);
		break;
	case EElementalType::Dark:
		ElementWeakness = EElementalType::Dark;
		ElementalWeaknessMesh->SetMaterial(0, M_Dark);
		break;
	}
}

void ASpiderEnemy::AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			if (!AttackCooldownActive)
			{
				UGameplayStatics::ApplyDamage(Sorcerer, Damage, GetController(), this, UDamageType::StaticClass());
				StartAttackCooldown();
			}
		}
	}
}

void ASpiderEnemy::AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// delete this function?
}

void ASpiderEnemy::StartAttackCooldown()
{
	GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &ASpiderEnemy::ClearAttackCooldown, AttackCooldownTime);
	AttackCooldownActive = true;
}

void ASpiderEnemy::ClearAttackCooldown()
{
	GetWorldTimerManager().ClearTimer(AttackCooldownTimer);
	AttackCooldownActive = false;
}