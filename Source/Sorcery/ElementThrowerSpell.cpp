// Fill out your copyright notice in the Description page of Project Settings.


#include "ElementThrowerSpell.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


AElementThrowerSpell::AElementThrowerSpell()
{
	ElementThrowerComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserSpell"));
	RootComponent = ElementThrowerComp;
	
	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageComp"));
	DamageSphere->SetupAttachment(RootComponent);

	SpellSound = CreateDefaultSubobject<UAudioComponent>(TEXT("SpellSound"));
	SpellSound->SetupAttachment(RootComponent);
	
	bThrowing = false;

	MaxAimDistance = 1000.f;
}

/*
	HELPERS
*/
FRotator AElementThrowerSpell::GetSpellOrientation(UCameraComponent* Camera)
{
	FVector AimPoint = GetAimLocation(Camera, MaxAimDistance);
	FVector Direction = AimPoint - ElementThrowerComp->GetComponentLocation();
	Direction.Normalize();
	return Direction.Rotation();
}

void AElementThrowerSpell::ChangeElementalType(EElementalType NewType)
{
	Super::ChangeElementalType(NewType);

	if (ElementThrowerComp->IsActive() && ElementThrowerComp->IsComplete())
	{
		ElementThrowerComp->Deactivate();
		ElementThrowerComp->Activate(true);
	}
	
	switch (Element)
	{
		case EElementalType::Fire:
			ElementThrowerComp->SetColorParameter(FName("ThrowerColor"), FireColor);
			break;
		case EElementalType::Shock:
			ElementThrowerComp->SetColorParameter(FName("ThrowerColor"), ShockColor);
			break;
		case EElementalType::Acid:
			ElementThrowerComp->SetColorParameter(FName("ThrowerColor"), AcidColor);
			break;
		case EElementalType::Dark:
			ElementThrowerComp->SetColorParameter(FName("ThrowerColor"), DarkColor);
			break;
	}
}

/*
	DAMAGE
*/
void AElementThrowerSpell::ShootThrowerSpell(ASorceryCharacter* Sorcerer)
{
	bThrowing = true;
	ElementThrowerComp->Activate();
	//ElementThrowerComp->SetBoolParameter(FName("LocalSpace"), true);
	ApplyThrowerDamage();
	SpellSound->Play();
}

void AElementThrowerSpell::ApplyThrowerDamage()
{
	// apply damage to all enemies in damage sphere
	TArray<AActor*> DamageableActors;
	DamageSphere->GetOverlappingActors(DamageableActors, AEnemy::StaticClass());
	for (AActor* Actor : DamageableActors)
	{
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if (Enemy)
		{
			ApplyDamageToEnemy(Enemy, nullptr, Damage, this, GetDamageType());
		}
	}
	
	GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &AElementThrowerSpell::ClearShootCooldown, AttackSpeed);
	bShootCooldownActive = true;
}

void AElementThrowerSpell::ClearShootCooldown()
{
	GetWorldTimerManager().ClearTimer(ShootCooldownTimer);
	bShootCooldownActive = false;

	if (bThrowing)
		ApplyThrowerDamage();
}

void AElementThrowerSpell::DeactivateThrowerSpell()
{
	bThrowing = false;
	ElementThrowerComp->Deactivate();
	//ElementThrowerComp->SetBoolParameter(FName("LocalSpace"), false);
	SpellSound->Stop();
}