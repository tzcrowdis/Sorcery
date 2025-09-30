// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserSpell.h"
#include "Enemy.h"
#include "SorceryCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

/*
	SETUP FUNCTIONS	
*/
ALaserSpell::ALaserSpell()
{
	LaserComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserSpell"));
	RootComponent = LaserComp;

	MaxLaserDistance = 5000.f;
}

void ALaserSpell::BeginPlay()
{
	Super::BeginPlay();

	//ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//if (!Sorcerer)
	//	return;

	//PlayerCamera = Sorcerer->GetFirstPersonCameraComponent();
}

void ALaserSpell::AttachLaser()
{
	// TODO setup location and stuff
	//maybe do in sorcery character
}

void ALaserSpell::ChangeElementalType(EElementalType NewType)
{
	Super::ChangeElementalType(NewType);

	LaserComp->Deactivate();
	LaserComp->Activate(true);

	switch (Element)
	{
		case EElementalType::Fire:
			LaserComp->SetColorParameter(FName("BeamColor"), FireColor);
			LaserComp->SetVariableMaterial(FName("OriginMat"), M_Fire);
			break;
		case EElementalType::Ice:
			LaserComp->SetColorParameter(FName("BeamColor"), IceColor);
			LaserComp->SetVariableMaterial(FName("OriginMat"), M_Ice);
			break;
		case EElementalType::Shock:
			LaserComp->SetColorParameter(FName("BeamColor"), ShockColor);
			LaserComp->SetVariableMaterial(FName("OriginMat"), M_Shock);
			break;
		case EElementalType::Acid:
			LaserComp->SetColorParameter(FName("BeamColor"), AcidColor);
			LaserComp->SetVariableMaterial(FName("OriginMat"), M_Acid);
			break;
	}
}


/*
*	ATTACK FUNCTIONS
*/
void ALaserSpell::ShootLaser(UCameraComponent* PlayerCamera)
{
	print("pressed");
	Camera = PlayerCamera;

	// Try and fire the laser
	if (LaserComp != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// get the hit point
			FHitResult HitResult;
			FVector Start = Camera->GetComponentLocation();
			FVector End = Start + Camera->GetForwardVector() * MaxLaserDistance;
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(this);
			GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
			
			FVector LaserEndPoint = HitResult.TraceEnd;
			if (HitResult.bBlockingHit)
				LaserEndPoint = HitResult.ImpactPoint;

			// enable the laser effect
			LaserComp->Activate();
			LaserFiring = true;

			// set beam end point
			LaserComp->SetVectorParameter(FName("BeamEnd"), LaserEndPoint);

			// apply damage if enemy is hit
			ApplyLaserDamage();
		}
	}

	/*
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	*/
}

void ALaserSpell::TickLaser()
{
	if (!LaserComp->IsActive())
		LaserComp->Activate();

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * MaxLaserDistance;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams);

	FVector LaserEndPoint = HitResult.TraceEnd;
	if (HitResult.bBlockingHit)
		LaserEndPoint = HitResult.ImpactPoint;

	LaserComp->SetVectorParameter(FName("BeamEnd"), LaserEndPoint);
}

void ALaserSpell::ApplyLaserDamage()
{
	// redo trace hit (probably expensive doing 2 traces in 1 frame)
	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * MaxLaserDistance;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams);

	print("apply damage");

	// apply damage if enemy is hit
	AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor());
	if (Enemy)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // NOTE assumes single player

		if (HitResult.GetComponent() == Enemy->WeakSpotComp)
		{
			Enemy->bWeakSpotHit = true;
			UGameplayStatics::ApplyDamage(Enemy, Damage * Enemy->WeakSpotMultiplier, PlayerController, this, GetDamageType());
		}
		else
		{
			Enemy->bWeakSpotHit = false;
			UGameplayStatics::ApplyDamage(Enemy, Damage, PlayerController, this, GetDamageType());
		}
	}

	// Only add impulse if we hit a physics object
	if ((HitResult.GetComponent() != nullptr) && HitResult.GetComponent()->IsSimulatingPhysics())
	{
		HitResult.GetComponent()->AddImpulseAtLocation(GetVelocity() * 100.0f, HitResult.ImpactPoint);
	}
}

void ALaserSpell::DestroyLaser()
{
	// destroy the laser component (or just disable)
	LaserComp->Deactivate();
	LaserFiring = false;
	print("released");
}