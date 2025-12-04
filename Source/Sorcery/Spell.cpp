// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SorceryCharacter.h"
#include "Enemy.h"
#include "Components/SphereComponent.h"

#include "DT_Fire.h"
#include "DT_Shock.h"
#include "DT_Dark.h"
#include "DT_Acid.h"

// Sets default values
ASpell::ASpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
	Damage = BaseDamage;
	AttackSpeed = BaseAttackSpeed;
}

// Called every frame
void ASpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpell::EquipSpell(EElementalType ElementType, float DamagePercent, float AttackSpeedPercent)
{
	ChangeElementalType(ElementType);
	UpdateDamage(DamagePercent);
	UpdateAttackSpeed(AttackSpeedPercent);
	UpdateReticle();
}

FHitResult ASpell::GetAimHitResult(UCameraComponent* PlayerCamera, float MaxAimDistance)
{
	FHitResult HitResult;
	FVector Start = PlayerCamera->GetComponentLocation();
	FVector End = Start + PlayerCamera->GetForwardVector() * MaxAimDistance;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
	return HitResult;
}

FVector ASpell::GetAimLocation(UCameraComponent* PlayerCamera, float MaxAimDistance)
{
	FHitResult HitResult;
	FVector Start = PlayerCamera->GetComponentLocation();
	FVector End = Start + PlayerCamera->GetForwardVector() * MaxAimDistance;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams);

	FVector AimPoint = End;
	if (HitResult.bBlockingHit)
		AimPoint = HitResult.Location;

	return AimPoint;
}

UClass* ASpell::GetDamageType()
{
	switch (Element)
	{
		case EElementalType::Fire:
			return UDT_Fire::StaticClass();
		case EElementalType::Shock:
			return UDT_Shock::StaticClass();
		case EElementalType::Acid:
			return UDT_Acid::StaticClass();
		case EElementalType::Dark:
			return UDT_Dark::StaticClass();
	}

	return nullptr;
}

void ASpell::ChangeElementalType(EElementalType NewType)
{
	Element = NewType;
}

void ASpell::UpdateDamage(float PercentOfBase)
{
	Damage = BaseDamage * PercentOfBase;
}

void ASpell::UpdateAttackSpeed(float PercentOfBase)
{
	AttackSpeed = BaseAttackSpeed * PercentOfBase;
}

void ASpell::ApplyModifiers(AActor* TargetActor)
{
	// NOTE assumes only one player
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	FLinearColor Color = FireColor;
	switch (Element)
	{
		case EElementalType::Fire:
			Color = FireColor;
			break;
		case EElementalType::Shock:
			Color = ShockColor;
			break;
		case EElementalType::Acid:
			Color = AcidColor;
			break;
		case EElementalType::Dark:
			Color = DarkColor;
			break;
	}

	Sorcerer->ApplyAllModifiers(TargetActor, GetDamageType(), Color);
}

void ASpell::ApplyDamageToEnemy(AEnemy* Enemy, UPrimitiveComponent* EnemyComp, float HitDamage, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	// NOTE i gotta be misunderstanding a collision setting but this should work
	//if (EnemyComp == Enemy->AttackSphere)
	//	return; 

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // NOTE assumes single player
	
	if (EnemyComp != nullptr && EnemyComp->ComponentHasTag("WeakSpot"))
	{
		Enemy->bWeakSpotHit = true;
		UGameplayStatics::ApplyDamage(Enemy, HitDamage * Enemy->WeakSpotMultiplier, PlayerController, DamageCauser, DamageTypeClass);
	}
	else
	{
		Enemy->bWeakSpotHit = false;
		UGameplayStatics::ApplyDamage(Enemy, HitDamage, PlayerController, DamageCauser, DamageTypeClass);
	}

	ApplyModifiers(Enemy);
}